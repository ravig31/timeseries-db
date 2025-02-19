#pragma once
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "chunk.h"
#include "datapoint.h"
#include "utils.h"

class Table
{
  public:
	size_t size() { return m_row_count; }
	void insert(const DataPoint& data);
	std::vector<DataPoint> const query(Timestamp start_ts, Timestamp end_ts);

	Table(const std::string& name, const std::string& data_path)
		: m_name(name)
		, m_row_count(0)
		, m_chunk_manager(ChunkManager(data_path))
	{
	}

  private:
	std::string m_name;
	std::vector<std::unique_ptr<Chunk>> m_chunks;
	std::map<int64_t, size_t> m_chunk_index;
	size_t m_row_count;
	ChunkManager m_chunk_manager;

	ChunkId generate_chunk_id();
};