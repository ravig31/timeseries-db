#include "table.h"
#include "datapoint.h"
#include "utils.h"
#include <cstddef>
#include <vector>

void Table::insert(const DataPoint& point)
{
	// Find chunk
	auto chunk = m_chunk_manager.get_chunk(point.ts);
	if (chunk == nullptr || chunk->is_full())
	{
		chunk = m_chunk_manager.create_chunk(point.ts, generate_chunk_id());
		// Capture the raw pointer before moving ownership
		Chunk* chunk_ptr = chunk.get();
		
		m_chunk_manager.insert_chunk(std::move(chunk));
		// Use the raw pointer to insert the data point
		chunk_ptr->append(point);
	}
	else
	{
		// Use the original unique_ptr to insert if no new chunk was created
		chunk->append(point);
	}

	m_row_count++;
}

ChunkId Table::generate_chunk_id() { return static_cast<ChunkId>(m_row_count); }

std::vector<DataPoint> const Table::query(Timestamp start_ts, Timestamp end_ts) {}
