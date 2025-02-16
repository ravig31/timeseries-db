#include "table.h"
#include "datapoint.h"
#include <vector>

void Table::create_chunk(DataPoint::Timestamp start_ts)
{
	m_chunks.push_back(std::make_unique<Chunk>(start_ts));
}

void Table::insert(const DataPoint& point)
{
	if (m_chunks.empty() || m_chunks.back()->is_full())
		create_chunk(point.ts);

	m_chunks.back()->insert(point);
	m_chunk_index[point.ts] = m_chunks.size();
}


std::vector<DataPoint> const Table::query(
	const std::string& table_name,
	DataPoint::Timestamp start_ts,
	DataPoint::Timestamp end_ts
)
{
	std::vector<DataPoint> results {};
	
	return results;
}

