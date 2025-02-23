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
	std::vector<DataPoint> results{};

	// Searh cache for chunks in range

	const ChunkTree& chunk_tree{ m_chunk_tree };
	const std::vector<ChunkFile*>& chunk_files = chunk_tree.range_query(q.m_time_range);

	// Load datapoints in parallell

	return results;
}

void Table::insert(const DataPoint& point)
{
	Timestamp partition_key = get_chunk_key(point.ts);
	auto it = m_chunk_cache.find(partition_key);

	if (it != m_chunk_cache.end())
	{
		Chunk* chunk = it->second.get();
		if (chunk && chunk->is_full())
		{
			evict_chunk(it);
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

Timestamp Table::get_chunk_key(Timestamp timestamp)
{
	return timestamp - (timestamp % (m_config.chunk_size_secs));
}

ChunkId Table::generate_chunk_id()
{
	static std::atomic<int64_t> next_id{ 1 };
	return next_id++;
}

std::unique_ptr<Chunk> Table::create_chunk(Timestamp start_ts)
{
	auto id = generate_chunk_id();
	auto chunk =
		std::make_unique<Chunk>(TimeRange{ start_ts, start_ts + m_config.chunk_size_secs }, id);
	return chunk;
}

void Table::create_and_cache_chunk(const Timestamp& partition_key, const DataPoint& point)
{
	std::unique_ptr<Chunk> new_chunk = create_chunk(partition_key);
	new_chunk->append(point);
	m_row_count++;
	m_chunk_cache[partition_key] = std::move(new_chunk);
}

void Table::evict_chunk(std::unordered_map<Timestamp, std::unique_ptr<Chunk>>::iterator& it)
{
	if (it != m_chunk_cache.end())
	{
		finalise_chunk(std::move(it->second));
		m_chunk_cache.erase(it);
	}
}

void Table::finalise_chunk(std::unique_ptr<Chunk> chunk)
{
	auto chunk_file = std::make_unique<ChunkFile>(m_data_path, chunk->id(), chunk->get_range());

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
