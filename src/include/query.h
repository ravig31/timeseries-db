#pragma once

#include "utils.h"
#include <cstddef>


struct Query
{
	Query(TimeRange range = TimeRange(), bool sorted = false, size_t limit = 0)
		: m_time_range(range)
		, m_sorted(sorted)
		, m_limit(limit)
	{
	}

	TimeRange m_time_range;
	bool m_sorted;
	size_t m_limit;
};
