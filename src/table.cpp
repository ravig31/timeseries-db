#include "table.h"

void Table::create_chunk(DataPoint::Timestamp start_ts)
{
	chunks.push_back(std::make_unique<Chunk>(start_ts));
}

void Table::insert(const DataPoint& point)
{
	if (chunks.empty() || chunks.back()->is_full())
		create_chunk(point.ts);

	chunks.back()->insert(point);
}
