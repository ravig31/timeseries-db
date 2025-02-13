#pragma once

#include <chrono>

class DataPoint
{
  public:
	using Timestamp = std::chrono::system_clock::time_point;

	Timestamp ts;
    double value;
};