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

	std::vector<DataPoint> const query(
		const DataPoint::Timestamp start,
		const DataPoint::Timestamp end
	);
	std::vector<DataPoint> const get_data(){ return data; }
	DataPoint::Timestamp get_start_ts(){ return start_ts; }
	DataPoint::Timestamp get_end_ts(){ return end_ts; }

  private:
	std::vector<DataPoint> data;
	DataPoint::Timestamp start_ts;
	DataPoint::Timestamp end_ts;
	size_t max_chunk_size{ 1000 };
};
