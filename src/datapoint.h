#pragma once

#include <cstdint>

struct DataPoint
{
	using Timestamp = int64_t;

	Timestamp ts;
	double value;

	int64_t encode_time_delta(Timestamp prev) const 
  {
    return ts - prev;
  }

  int64_t encode_value() const
  {
    // make value fixed point with 4 decimals 
    return static_cast<int64_t>(value*1000);
  }
};