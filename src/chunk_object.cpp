#include "chunk.h"
#include "datapoint.h"
#include <cstddef>
#include <cstdint>
#include <vector>

void Chunk::append(const DataPoint& point)
{
	if (is_full())
		return;

	int64_t timedelta = point.encode_time_delta(m_start_ts);
	m_ts_deltas.push_back(timedelta);
	m_values.push_back(point.value);
	m_points++;
	m_is_to_save = true;
}

