#pragma once

#include <cstdint>
#include <string>

struct DataPoint
{

	int64_t ts;
	double value;
  std::string table_id;

	int64_t encode_time_delta(int64_t prev) const 
  {
    return ts - prev;
  }
};