#include "chunk.h"
#include "datapoint.h"
#include <vector>

void Chunk::insert(const DataPoint& point)
{
	if (data.empty())
		start_ts = point.ts;
	data.push_back(point);
	end_ts = point.ts;
}

std::vector<DataPoint> Chunk::query(const DataPoint::Timestamp start_ts, const DataPoint::Timestamp end_ts)
{
    std::vector<DataPoint> results;
    for (const auto& point : data) {
        if (point.ts >= start_ts && point.ts <= end_ts)
            results.push_back(point);
    }

    return results;
}