#include "table.h"
#include "chunk.h"
#include "datapoint.h"
#include "utils.h"
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

std::vector<DataPoint> Table::query(const Query& q) const
{
	// consider using weak_ptrs here
	auto chunk_files = m_chunk_tree.range_query(q.m_time_range);
	std::vector<std::shared_ptr<Chunk>> chunks{};

	for (const auto& file : chunk_files)
	{
		Timestamp key = file->get_metadata().chunk_range.end_ts;
		auto it = m_chunk_cache.find(key);
		if (it != m_chunk_cache.end())
		{
			chunks.push_back(it->second);
		}
		else
		{
			auto chunk = file->load();
			if (chunk != nullptr)
				chunks.push_back(std::move(chunk));
		}
	}
	// Add load datapoints in parallell
	auto results = gather_data_from_chunks(chunks, q.m_time_range);
	return results;
}


void Table::insert(const DataPoint& point)
{
	Timestamp partition_key = get_partition_key(point.ts);
	auto it = m_chunk_cache.find(partition_key);

	if (it != m_chunk_cache.end())
	{
		Chunk* chunk = it->second.get();
		if (chunk && chunk->is_full())
		{
			if (it != m_chunk_cache.end())
			{
				finalise_chunk(std::move(it->second));
				m_chunk_cache.erase(it);
			}
			create_and_cache_chunk(partition_key, point);
		}
		else if (chunk)
		{
			chunk->append(point);
			m_row_count++;
		}
	}
	else
	{
		create_and_cache_chunk(partition_key, point);
	}
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
		id
	);
	return chunk;
}

void Table::create_and_cache_chunk(const Timestamp& partition_key, const DataPoint& point)
{
	auto new_chunk = create_chunk(partition_key);
	new_chunk->append(point);
	m_row_count++;
	m_chunk_cache[partition_key] = std::move(new_chunk);
}

void Table::finalise_chunk(const std::shared_ptr<Chunk>& chunk)
{
	auto chunk_file =
		std::make_shared<ChunkFile>(m_data_path, chunk->id(), chunk->get_range(), chunk->size());

	// Insert into tree first and get a reference to the stored ChunkFile
	ChunkFile& stored_file = *chunk_file; // Get reference before moving
	m_chunk_tree.insert(chunk->get_range(), std::move(chunk_file));

	// Store reference to the ChunkFile along with the chunk
	m_chunks_to_save.emplace_back(&stored_file, std::move(chunk));
}

void Table::flush_chunks()
{
	for (const auto& [chunk_file_ref, chunk] : m_chunks_to_save)
	{
		// Use .get() on reference_wrapper to get the actual reference
		chunk_file_ref->save(*chunk);
		}
	m_chunks_to_save.clear(); // Clear after saving
}
