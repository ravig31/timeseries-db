#pragma once

#include <cstddef>
#include <cstdint>
namespace Config
{
constexpr size_t MAX_NODE_SIZE{ 4 }; // Maximum amount children per node (2*t)
constexpr size_t MAX_CHUNK_SIZE{ 1 };
constexpr size_t CHUNK_CACHE_SIZE{ 1 };
constexpr size_t MAX_CHUNKS_TO_SAVE{ 6 };
constexpr int64_t FLUSH_INTERVAL_SECS { 60 };
constexpr int64_t CHUNK_INTERVAL_SECS{ 3600 };
} // namespace Config