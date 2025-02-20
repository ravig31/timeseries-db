#include "table.h"
#include "chunk.h"
#include "config.h"
#include "datapoint.h"
#include "utils.h"
#include <cstddef>
#include <memory>
#include <vector>

std::vector<DataPoint> const Table::query(Timestamp start_ts, Timestamp end_ts) {}

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
		}
	}
	else
	{
		create_and_cache_chunk(partition_key, point);
	}
}

Timestamp Table::get_chunk_key(Timestamp timestamp)
{
	return timestamp - (timestamp % (Config::CHUNK_INTERVAL_SECS));
}

ChunkId Table::generate_chunk_id()
{
	static std::atomic<int64_t> next_id{ 1 };
	return next_id++;
}

std::unique_ptr<Chunk> Table::create_chunk(Timestamp start_ts)
{
	auto id = generate_chunk_id();
	auto chunk = std::make_unique<Chunk>(start_ts, id);
	return chunk;
}

void Table::evict_chunk(std::unordered_map<Timestamp, std::unique_ptr<Chunk>>::iterator& it)
{
	std::unique_ptr<Chunk> full_chunk = std::move(it->second);
	m_chunk_cache.erase(it);
	finalise_chunk(std::move(full_chunk));
}

void Table::create_and_cache_chunk(const Timestamp& partition_key, const DataPoint& point)
{
	std::unique_ptr<Chunk> new_chunk = create_chunk(partition_key);
	new_chunk->append(point);
	m_chunk_cache[partition_key] = std::move(new_chunk);
}

void Table::finalise_chunk(std::unique_ptr<Chunk> chunk)
{
	auto chunk_file = std::make_unique<ChunkFile>(m_data_path, chunk->id(), chunk->get_start_ts());
	// Persist chunk contents
	chunk_file->save(*chunk);
	// Save chunk file pointer in tree for query
	m_chunk_tree.insert(chunk->get_start_ts(), std::move(chunk_file));
}