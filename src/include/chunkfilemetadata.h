#pragma once

#include "utils.h"

struct ChunkMetadata
{
	ChunkId chunk_id;
	TimeRange chunk_range;
	size_t row_count;
	size_t capacity;
};
