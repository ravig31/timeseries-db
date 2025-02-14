#pragma once

#include <chrono>

struct DataPoint
{
	using Timestamp = std::chrono::system_clock::time_point;

	Timestamp ts;
	double value;
};