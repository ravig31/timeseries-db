#pragma once
#include <memory>
#include <string>
#include <vector>

#include "chunk.h"
#include "datapoint.h"

class Table
{
  public:
	struct Schema
	{
		std::vector<std::string> fields;
	};

	void insert(const DataPoint& data);
	void create_chunk(DataPoint::Timestamp start_ts);
	std::vector<DataPoint> const query(
		const std::string& table_name,
		DataPoint::Timestamp start_ts,
		DataPoint::Timestamp end_ts
	);

	std::vector<std::unique_ptr<Chunk>>& get_chunks(){ return chunks; }

	Table(const std::string& name)
		: m_name(name)
	{
	}

  private:
	std::string m_name;
	std::vector<std::unique_ptr<Chunk>> chunks;
};