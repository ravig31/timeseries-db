
#pragma once
#include <cstdint>

using Timestamp = int64_t;
using ChunkId = int64_t;

struct TimeRange {
    Timestamp start_ts;
    Timestamp end_ts;
    
    bool contains(int64_t timestamp) const {
        return timestamp >= start_ts && timestamp <= end_ts;
    }

    void reset() 
    {
        start_ts = 0;
        end_ts = 0;
    }
};
