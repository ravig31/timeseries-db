#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "chunk.h"

class Table
{
  public:
	Table(const std::string& name, const std::string& data_path)
		: m_name(name)
		, m_data_path(data_path)
		, m_row_count(0)
		, m_chunk_tree(ChunkTree(data_path))
	{
	}
	size_t size() { return m_row_count; }
	void insert(const DataPoint& data);
	std::vector<DataPoint> const query(Timestamp start_ts, Timestamp end_ts);

  private:
	std::string m_name;
	std::string m_data_path;
	size_t m_row_count;
	ChunkTree m_chunk_tree;
	std::unordered_map<Timestamp, std::unique_ptr<Chunk>> m_chunk_cache;

	Timestamp get_chunk_key(Timestamp timestamp);
	ChunkId generate_chunk_id();
	
	std::unique_ptr<Chunk> create_chunk(Timestamp ts);
	void evict_chunk(std::unordered_map<Timestamp, std::unique_ptr<Chunk>>::iterator& it);
	void create_and_cache_chunk(const Timestamp& partition_key, const DataPoint& point);

	void finalise_chunk(std::unique_ptr<Chunk> chunk);
};