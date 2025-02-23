#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "chunk.h"
#include "tree.h"
#include "datapoint.h"
#include "query.h"

class Table
{
  public:
	struct Config 
	{
		const int64_t chunk_size_secs;
		const size_t chunk_cache_size;
		const size_t max_chunks_to_save;
		const int64_t flush_interval_secs;
	};

	Table(const std::string& name, const std::string& data_path, const Table::Config& config)
		: m_name(name)
		, m_data_path(data_path)
		, m_row_count(0)
		, m_config(config)
		, m_chunk_tree(ChunkTree(data_path, config.chunk_size_secs))
	{
	}

	~Table() { flush_chunks(); }

	size_t rows() { return m_row_count; }

	std::vector<DataPoint> query(const Query& q) const ;
	void insert(const DataPoint& data);

	void flush_chunks();

  private:

	std::string m_name;
	std::string m_data_path;
	size_t m_row_count;
	Config m_config;

	ChunkTree m_chunk_tree;
	std::unordered_map<Timestamp, std::unique_ptr<Chunk>> m_chunk_cache;
	std::vector<std::pair<ChunkFile*, std::unique_ptr<Chunk>>> m_chunks_to_save;

	Timestamp get_chunk_key(Timestamp timestamp);
	ChunkId generate_chunk_id();

	std::unique_ptr<Chunk> create_chunk(Timestamp ts);
	void evict_chunk(std::unordered_map<Timestamp, std::unique_ptr<Chunk>>::iterator& it);
	void create_and_cache_chunk(const Timestamp& partition_key, const DataPoint& point);

	void finalise_chunk(std::unique_ptr<Chunk> chunk);
	void flush_loop();

	friend class QueryProcessor;
};
