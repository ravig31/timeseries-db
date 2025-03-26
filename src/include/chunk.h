#pragma once

#include "datapoint.h"
#include "utils.h"
#include "chunkfilemetadata.h"

#include <vector>
#include <stdexcept>

class Chunk
{
  public:
	Chunk(TimeRange range, ChunkId id, size_t capacity)
		: m_range(range)
		, m_id(id)
		, m_capacity(capacity)
		, m_row_count(0)
		, m_is_to_save(false)

	{
		if (capacity == 0)
		{
			throw std::invalid_argument("Initial capacity must be greater than zero.");
		}

		m_ts_deltas.reserve(capacity);
		m_values.reserve(capacity);
	}
	Chunk(
		const ChunkMetadata& metadata,
		std::vector<Timestamp>&& deltas,
		std::vector<double>&& values
	)
		: m_range(metadata.chunk_range)
		, m_id(metadata.chunk_id)
		, m_capacity(metadata.capacity)
		, m_row_count(metadata.row_count)
		, m_is_to_save(false)
		, m_ts_deltas(std::move(deltas))
		, m_values(std::move(values))
	{
	}

	std::vector<DataPoint> get_data_in_range(const TimeRange& range) const;
	void append(const DataPoint& point);

	ChunkId id() const { return m_id; }
	const TimeRange& get_range() const { return m_range; }
	bool is_to_save() const { return m_is_to_save; }
	void set_to_save(bool is_to_save) { m_is_to_save = is_to_save; }
	size_t size() const { return m_row_count; }
	size_t capacity() const { return m_capacity; }
	bool is_full() const { return m_ts_deltas.size() >= m_capacity; }
	void mark_to_save() { m_is_to_save = true; }
	void unmark_to_save() { m_is_to_save = false; }

  private:
	const TimeRange m_range;
	const ChunkId m_id;
	const size_t m_capacity;
	size_t m_row_count;
	bool m_is_to_save;

	std::vector<Timestamp> m_ts_deltas;
	std::vector<double> m_values;
	friend class ChunkFile;
};

