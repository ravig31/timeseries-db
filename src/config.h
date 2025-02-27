#pragma once

#include "utils.h"
#include <cstddef>
namespace Config
{
constexpr size_t MAX_NODE_SIZE{ 127 }; // Maximum amount keys per node (2*t-1)
constexpr size_t MAX_CHUNK_SIZE{ 30 };
constexpr size_t CHUNK_CACHE_SIZE{ 1 };
constexpr size_t MAX_CHUNKS_TO_SAVE{ 6 };
constexpr TimeDelta FLUSH_INTERVAL_SECS{ 60 };
constexpr TimeDelta CHUNK_INTERVAL_SECS{ 3600 };
constexpr TimeDelta MIN_DATA_RESOLUTION_SECS{ 1 };
} // namespace Config