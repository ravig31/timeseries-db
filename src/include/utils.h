#pragma once
#include <cstdint>

using Timestamp = int64_t;
using TimeDelta = int64_t;
using ChunkId = int64_t;

constexpr Timestamp TIMESTAMP_MIN = 0;
constexpr Timestamp TIMESTAMP_MAX = 253402300799; // Year 9999 in Unix timestamp

struct TimeRange {
	Timestamp start_ts;
	Timestamp end_ts;

	TimeRange() : start_ts(TIMESTAMP_MIN), end_ts(TIMESTAMP_MAX) {}

	TimeRange(Timestamp start, Timestamp end) : start_ts(start), end_ts(end) {}

	TimeRange(Timestamp start) : start_ts(start), end_ts(TIMESTAMP_MAX) {}

	TimeDelta duration() const { return end_ts - start_ts; }
	bool contains(Timestamp timestamp) const {
		return timestamp >= start_ts && timestamp <= end_ts;
	}
	bool overlaps(TimeRange range) const {
		return start_ts <= range.end_ts && range.start_ts < end_ts;
	}
};