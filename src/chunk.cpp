#include "chunk.h"
#include "datapoint.h"
#include <cstdint>
#include <fstream>
#include <vector>

void Chunk::insert(const DataPoint& point)
{
	if (is_full())
		return;

	int64_t timedelta =
		point.encode_time_delta(m_ts_deltas.empty() ? m_base_ts : m_base_ts + m_ts_deltas.back());
	m_ts_deltas.push_back(timedelta);
	m_encoded_values.push_back(point.encode_value());
}

std::vector<DataPoint> const Chunk::query(
	const DataPoint::Timestamp start_ts,
	const DataPoint::Timestamp end_ts
)
{
	std::vector<DataPoint> results{};
	int64_t point_ts{ m_base_ts };
	for (size_t i{ 0 }; i < m_ts_deltas.size(); i++)
	{
		point_ts += m_ts_deltas[i];
		if (point_ts >= start_ts && point_ts <= end_ts)
		{
			results.push_back(DataPoint{ point_ts, static_cast<double>(m_encoded_values[i]) });
		}
	}
    return results;
}

void Chunk::persist(const std::string& path) const
{
	std::ofstream outf(path, std::ios::out | std::ios::trunc);

	if (!outf.is_open())
	{ // Check with is_open()
		throw std::runtime_error(
			"Failed to open file for persisting chunk: " + path
		); // Include path in error
	}

	ChunkHeader header{ m_base_ts, static_cast<uint32_t>(m_ts_deltas.size()) };

	// Write the header (using proper casting for time_point)
	outf.write(reinterpret_cast<const char*>(&header), sizeof(header));
	if (outf.fail())
	{
		outf.close(); // Close on error
		throw std::runtime_error("Failed to write chunk header to file: " + path);
	}

	// Write the time deltas (assuming they are some kind of numeric type)
	if (!m_ts_deltas.empty())
	{
		outf.write(
			reinterpret_cast<const char*>(m_ts_deltas.data()),
			m_ts_deltas.size() * sizeof(decltype(m_ts_deltas)::value_type)
		);
		if (outf.fail())
		{
			outf.close();
			throw std::runtime_error("Failed to write chunk deltas to file: " + path);
		}
	}
	// Write data
	if (!m_encoded_values.empty())
	{
		outf.write(
			reinterpret_cast<const char*>(m_encoded_values.data()),
			m_encoded_values.size() * sizeof(decltype(m_ts_deltas)::value_type)
		);
		if (outf.fail())
		{
			outf.close();
			throw std::runtime_error("Failed to write chunk data to file: " + path);
		}
	}

	outf.close();
}