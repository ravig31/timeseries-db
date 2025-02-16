#pragma once
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "chunk.h"
#include "datapoint.h"

class Table
{
  public:
	std::vector<std::unique_ptr<Chunk>> m_chunks;
	struct Schema
	{
		std::vector<std::string> fields;
	};
	std::vector<std::unique_ptr<Chunk>>& get_chunks() { return m_chunks; }
	size_t size() const
	{
		size_t totalSize = 0;
		for (const auto& chunk : m_chunks)
		{
			if (chunk)
			{
				totalSize += chunk->size();
			}
		}
		return totalSize;
	}

	void insert(const DataPoint& data);
	void create_chunk(DataPoint::Timestamp start_ts);
	std::vector<DataPoint> const query(
		const std::string& table_name,
		DataPoint::Timestamp start_ts,
		DataPoint::Timestamp end_ts
	);

	Table(const std::string& name)
		: m_name(name)
	{
	}

  private:
	std::string m_name;
	std::map<DataPoint::Timestamp, size_t> m_chunk_index;
};