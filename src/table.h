#pragma once
#include <cstddef>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "chunk.h"
#include "datapoint.h"
#include "query.h"
#include "tree.h"
#include "utils.h"

class Table
{
  public:
	struct Config
	{
		const TimeDelta chunk_size_secs;
		const size_t chunk_capacity;
		const size_t chunk_cache_size;
		const size_t max_chunks_to_save;
		const TimeDelta flush_interval_secs;
		const TimeDelta min_resolution_secs;
	};

	Table(const std::string& name, const std::string& data_path, const Table::Config& config)
		: m_name(name)
		, m_data_path(data_path)
		, m_row_count(0)
		, m_config(config)
		, m_chunk_tree(ChunkTree(data_path, config.chunk_size_secs))
	{
	}

	size_t rows() { return m_row_count; }

	std::vector<DataPoint> query(const Query& q);
	void insert(const DataPoint& data);

	void finalise_all();
	void flush_chunks();

  private:
	std::string m_name;
	std::string m_data_path;
	size_t m_row_count;
	Config m_config;
	Timestamp m_latest_point_ts;

	// Querying
	ChunkTree m_chunk_tree;
	std::vector<DataPoint> gather_data_from_chunks(
		const std::vector<std::shared_ptr<Chunk>>& chunks,
		const TimeRange& query_range
	) const;
	
	// Utils
	Timestamp get_partition_key(Timestamp timestamp);
	ChunkId generate_chunk_id();
	
	// Creation
	std::shared_ptr<Chunk> create_chunk(Timestamp partition_key);
	
	// Caching
	std::unordered_map<Timestamp, std::pair<std::shared_ptr<Chunk>, std::list<Timestamp>::iterator>>
		m_chunk_cache;
	std::list<Timestamp> m_chunk_cache_usage_list;
	std::shared_ptr<Chunk> get_chunk_from_cache(Timestamp partition_key);
    void put_chunk_in_cache(Timestamp partition_key, std::shared_ptr<Chunk> chunk);
	void evict_from_cache(Timestamp partition_key);
	
	std::vector<std::pair<std::weak_ptr<ChunkFile>, std::shared_ptr<Chunk>>> m_chunks_to_save;
	void finalise_chunk(std::shared_ptr<Chunk> chunk);
};
