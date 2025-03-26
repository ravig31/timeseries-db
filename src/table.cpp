
#include <cassert>
#include <cstddef>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread_pool/thread_pool.h>
#include <utility>
#include <vector>

#include "chunk.h"
#include "chunkfile.h"
#include "datapoint.h"
#include "query.h"
#include "table.h"
#include "tree.h"

std::vector<DataPoint> Table::query(const Query& q)
{
	auto chunk_files = m_chunk_tree.range_query(q.m_time_range);
	std::vector<std::shared_ptr<Chunk>> chunks;
	chunks.reserve(chunk_files.size());
	std::vector<std::future<std::shared_ptr<Chunk>>> chunk_futures{};
	chunk_futures.reserve(chunk_files.size());

	dp::thread_pool pool(6);
	for (const auto& file : chunk_files)
	{
		Timestamp key = file->get_metadata().chunk_range.end_ts;
		auto chunk = get_chunk_from_cache(key);

		if (chunk)
		{
			chunks.push_back(chunk);
			m_metrics.m_cache_hits++;
		}
		else
		{
			m_metrics.m_cache_misses++;
			// Add load datapoints into thread pool;
			auto task = [this, file, key]()
			{
				auto chunk = file->load();
				if (chunk)
				{

					std::shared_ptr<Chunk> shared_chunk(std::move(chunk));
					put_chunk_in_cache(key, shared_chunk);
					return shared_chunk;
				}
				return std::shared_ptr<Chunk>();
			};
			chunk_futures.push_back(pool.enqueue(task));
		}
	}

	for (auto& future : chunk_futures)
	{
		auto chunk = future.get();
		if (chunk)
		{
			chunks.push_back(chunk);
		}
	}

	auto results = gather_data_from_chunks(chunks, q.m_time_range);
	return results;
}

void Table::insert(const std::vector<DataPoint>& points)
{
	for (const auto& point: points)
	{
		insert_single(point);
		if (m_chunks_to_save.size() == m_config.max_chunks_to_save)
			flush_chunks();
	}
	// Perist remaining from cache
	finalise_all();
	flush_chunks();
}

void Table::insert_single(const DataPoint& point)
{
	assert(point.ts > m_latest_point_ts && "Does not support historical data implementation");
	m_latest_point_ts = point.ts;

	Timestamp partition_key = get_partition_key(point.ts);
	auto chunk = get_chunk_from_cache(partition_key);

	// Uses write behind cache
	if (chunk != nullptr)
	{
		chunk->append(point);
	}
	else
	{
		auto new_chunk = create_chunk(partition_key);
		new_chunk->append(point);
		put_chunk_in_cache(partition_key, std::move(new_chunk));
	}
	m_row_count++;
}

std::vector<DataPoint> Table::gather_data_from_chunks(
	const std::vector<std::shared_ptr<Chunk>>& chunks,
	const TimeRange& query_range
) const
{
	std::vector<DataPoint> results{};
	results.reserve(
		query_range.duration() / (m_config.chunk_size_secs / m_config.min_resolution_secs)
	);
	for (const auto& chunk : chunks)
	{
		auto data = chunk->get_data_in_range(query_range);
		results.insert(results.end(), data.begin(), data.end());
	}

	return results;
}

Timestamp Table::get_partition_key(Timestamp timestamp)
{
	Timestamp current_chunk_start = timestamp - (timestamp % m_config.chunk_size_secs);
	return current_chunk_start + m_config.chunk_size_secs;
}

ChunkId Table::generate_chunk_id()
{
	static std::atomic<int64_t> next_id{ 1 };
	return next_id++;
}

std::shared_ptr<Chunk> Table::create_chunk(Timestamp partition_key)
{
	auto id = generate_chunk_id();
	auto chunk = std::make_shared<Chunk>(
		TimeRange{ partition_key - m_config.chunk_size_secs, partition_key },
		id,
		m_config.chunk_capacity
	);
	return chunk;
}

std::shared_ptr<Chunk> Table::get_chunk_from_cache(Timestamp partition_key)
{
	std::lock_guard<std::mutex> lock(m_cache_mutex);
	auto it = m_chunk_cache.find(partition_key);
	if (it != m_chunk_cache.end())
	{
		m_chunk_cache_usage_list.erase(it->second.second);
		m_chunk_cache_usage_list.push_front(partition_key);
		it->second.second = m_chunk_cache_usage_list.begin();
		return it->second.first;
	}
	return nullptr;
}

void Table::put_chunk_in_cache(Timestamp partition_key, std::shared_ptr<Chunk> chunk)
{
	std::lock_guard<std::mutex> lock(m_cache_mutex);

	auto it = m_chunk_cache.find(partition_key);
	if (it != m_chunk_cache.end())
	{
		m_chunk_cache_usage_list.erase(it->second.second);
		m_chunk_cache_usage_list.push_front(partition_key);
		it->second.second = m_chunk_cache_usage_list.begin();
	}
	else
	{
		if (m_chunk_cache.size() >= m_config.chunk_cache_size)
		{
			Timestamp lru_key = m_chunk_cache_usage_list.back();
			evict_from_cache(lru_key);
		}
		m_chunk_cache_usage_list.push_front(partition_key);
		m_chunk_cache[partition_key] = { std::move(chunk), m_chunk_cache_usage_list.begin() };
	}
}

void Table::evict_from_cache(Timestamp partition_key)
{
	auto it = m_chunk_cache.find(partition_key);
	m_chunk_cache_usage_list.erase(it->second.second);
	finalise_single(std::move(it->second.first));
	m_chunk_cache.erase(it);
}

void Table::finalise_single(std::shared_ptr<Chunk> chunk)
{
	std::lock_guard<std::mutex> lock(m_flush_mutex);
	auto chunk_file = std::make_shared<ChunkFile>(
		m_data_path,
		chunk->id(),
		chunk->get_range(),
		chunk->size(),
		chunk->capacity()
	);

	// Insert into tree and get a reference to the stored ChunkFile
	m_chunk_tree.insert(chunk->get_range(), chunk_file); // Insert the shared_ptr

	// Store weak pointer to the ChunkFile along with the chunk
	m_chunks_to_save.emplace_back(std::weak_ptr<ChunkFile>(chunk_file), std::move(chunk));
}

void Table::finalise_all()
{
	for (auto [_, pair] : m_chunk_cache)
	{
		finalise_single(std::move(pair.first));
	}
}

void Table::flush_chunks()
{
	for (auto& [chunk_file_weak, chunk] : m_chunks_to_save)
	{
		finalise_single(chunk_file_weak, chunk);
	}
	m_chunks_to_save.clear();
}


void Table::finalise_single(std::weak_ptr<ChunkFile> chunk_file, std::shared_ptr<Chunk> chunk)
{
	std::lock_guard<std::mutex> lock(m_flush_mutex);
	if (auto chunk_file_shared = chunk_file.lock())
	{
		chunk_file_shared->save(*chunk);
	}
	else
	{
		// the weak pointer has expired. Skip this chunk.
		std::cerr << "Chunk file does not exist." << "\n";
	}
}