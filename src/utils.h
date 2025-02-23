
#pragma once
#include <cstdint>
#include <limits>

using Timestamp = int64_t;
using ChunkId = int64_t;

struct TimeRange
{

	Timestamp start_ts;
	Timestamp end_ts;

	TimeRange()
		: start_ts(0)
		, end_ts(std::numeric_limits<Timestamp>::max())

	{
	}

	TimeRange(Timestamp start, Timestamp end)
		: start_ts(start)
		, end_ts(end)
	{
	}
	TimeRange(Timestamp start)
		: start_ts(start)
		, end_ts(std::numeric_limits<Timestamp>::max())
	{
	}

	int64_t duration() const { return end_ts - start_ts; }
	bool contains(int64_t timestamp) const { return timestamp >= start_ts && timestamp <= end_ts; }
};