#pragma once

#include "datapoint.h"
#include <cstdint>
#include <sys/types.h>
#include <vector>

class Chunk
{
  public:
	Chunk(DataPoint::Timestamp start)
		: m_base_ts(start)
	{
		m_ts_deltas.reserve(MAX_CHUNK_SIZE);
		m_encoded_values.reserve(MAX_CHUNK_SIZE);
	};
	std::vector<int64_t> const get_data() { return m_encoded_values; }
	DataPoint::Timestamp get_base_ts() { return m_base_ts; }
	size_t size() { return m_ts_deltas.size(); }
	bool is_full() const { return m_ts_deltas.size() >= MAX_CHUNK_SIZE; }
	void insert(const DataPoint& point);

	std::vector<DataPoint> const query(
		const DataPoint::Timestamp start,
		const DataPoint::Timestamp end
	);

	void persist(const std::string& path) const;

  private:
	static constexpr size_t MAX_CHUNK_SIZE{ 1000 };
	std::vector<int64_t> m_ts_deltas;
	std::vector<int64_t> m_encoded_values;
	DataPoint::Timestamp m_base_ts;

	struct ChunkHeader
	{
		DataPoint::Timestamp start_ts;
		uint32_t count;
	};
};
