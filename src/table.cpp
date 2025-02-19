#include "table.h"
#include "datapoint.h"
#include "utils.h"
#include <cstddef>
#include <vector>



void Table::insert(const DataPoint& point)
{
	//Find chunk
	auto chunk = m_chunk_manager.get_chunk(point.ts);
	if (chunk->is_full())
	{
		chunk = m_chunk_manager.create_chunk(point.ts);
		m_chunk_manager.insert_chunk(std::move(chunk));
	}

	chunk->insert(point);
	m_row_count++;
}

std::vector<DataPoint> const Table::query(Timestamp start_ts, Timestamp end_ts)
{
}
