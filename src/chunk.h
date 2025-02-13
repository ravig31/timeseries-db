#pragma once

#include "datapoint.h"
#include <vector>

class Chunk
{
  public:
	Chunk(DataPoint::Timestamp start)
		: start_ts(start) {};

	bool is_full() const { return data.size() >= max_chunk_size; }

	void insert(const DataPoint& point);

	std::vector<DataPoint> query(const DataPoint::Timestamp start, const DataPoint::Timestamp end);

  private:
	std::vector<DataPoint> data;
	DataPoint::Timestamp start_ts;
	DataPoint::Timestamp end_ts;
	size_t max_chunk_size{ 1000 };
};
