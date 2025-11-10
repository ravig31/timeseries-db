#pragma once

#include "utils.h"

struct DataPoint {
Timestamp ts;
  double value;

  TimeDelta encode_time_delta(Timestamp base_ts) const {
      return ts - base_ts;
  }

  // Overload the less-than operator for DataPoint
  bool operator<(const DataPoint& other) const {
      return ts < other.ts;
  }
};