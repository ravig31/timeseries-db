#pragma once

#include "utils.h"
#include <cstddef>
namespace Config
{
constexpr size_t MAX_NODE_SIZE{ 4 }; // Maximum amount children per node (2*t)
constexpr size_t MAX_CHUNK_SIZE{ 12 };
constexpr size_t CHUNK_CACHE_SIZE{ 12};
constexpr size_t MAX_CHUNKS_TO_SAVE{ 6 };
constexpr TimeDelta FLUSH_INTERVAL_SECS{ 60 };
constexpr TimeDelta CHUNK_INTERVAL_SECS{ 3600 };
constexpr TimeDelta MIN_DATA_RESOLUTION_SECS{ 1 };
} // namespace Config