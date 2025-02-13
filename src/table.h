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
	std::vector<DataPoint> const query();

  private:
	std::string name;
	std::vector<std::unique_ptr<Chunk>> chunks;
};