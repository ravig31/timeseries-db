#include "chunk.h"
#include "chunkfile.h"

#include <cstddef>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <memory>


std::vector<DataPoint> Chunk::get_data_in_range(const TimeRange& range) const
{
	// Could Optimise
	std::vector<DataPoint> results{};
	results.reserve(m_capacity);
	for (size_t i{ 0 }; i < size(); i++)
	{
		const auto ts = m_range.start_ts + m_ts_deltas[i];
		if (range.contains(ts))
			results.push_back(DataPoint{ ts, m_values[i] });
	}
	return results;
}

void Chunk::append(const DataPoint& point)
{
	if (is_full())
	{
		m_is_to_save = true;
		return;
	}

	TimeDelta timedelta = point.encode_time_delta(m_range.start_ts);
	m_ts_deltas.push_back(timedelta);
	m_values.push_back(point.value);
	m_row_count++;
}

void ChunkFile::save(const Chunk& chunk) const
{
	std::ofstream outf(m_chunk_path, std::ios::binary);
	if (!outf.is_open())
	{
		throw std::runtime_error("Failed to open chunk file for saving: " + m_chunk_path);
	}

	try
	{
		write_metadata(outf, chunk);
		write_deltas(outf, chunk.m_ts_deltas);
		write_values(outf, chunk.m_values);
	}
	catch (const std::exception& e)
	{
		outf.close();
		throw std::runtime_error("Error writing chunk data: " + std::string(e.what()));
	}

	outf.close();
}

std::unique_ptr<Chunk> ChunkFile::load() const
{
	std::ifstream inf(m_chunk_path, std::ios::binary);
	if (!inf.is_open())
	{
		throw std::runtime_error("Failed to open chunk file for loading: " + m_chunk_path);
	}

	ChunkMetadata metadata;
	std::vector<Timestamp> deltas;
	std::vector<double> values;

	try
	{
		metadata = read_metadata(inf);
		deltas = read_deltas(inf);
		values = read_values(inf);
	}
	catch (const std::exception& e)
	{
		inf.close();
		throw std::runtime_error("Error reading chunk data: " + std::string(e.what()));
	}

	// Create and return a Chunk object
	std::unique_ptr<Chunk> chunk =
		std::make_unique<Chunk>(metadata, std::move(deltas), std::move(values));
	return chunk;
}

void ChunkFile::write_metadata(std::ofstream& file, const Chunk& chunk)
{
	ChunkMetadata metadata{ chunk.id(), chunk.get_range(), chunk.size(), chunk.capacity() };
	file.write(reinterpret_cast<const char*>(&metadata), sizeof(metadata));
	if (file.fail())
	{
		throw std::runtime_error("Failed to write chunk metadata");
	}
}

ChunkMetadata ChunkFile::read_metadata(std::ifstream& file)
{
	ChunkMetadata metadata;

	file.read(reinterpret_cast<char*>(&metadata), sizeof(metadata));

	if (file.fail())
	{
		throw std::runtime_error("Failed to read chunk metadata");
	}

	return metadata;
}

void ChunkFile::write_deltas(std::ofstream& file, const std::vector<Timestamp>& deltas)
{
	// Write the number of deltas
	size_t num_deltas = deltas.size();
	file.write(reinterpret_cast<const char*>(&num_deltas), sizeof(num_deltas));
	if (file.fail())
	{
		throw std::runtime_error("Failed to write number of deltas");
	}

	// Then write the actual delta
	file.write(reinterpret_cast<const char*>(deltas.data()), num_deltas * sizeof(Timestamp));
	if (file.fail())
	{
		throw std::runtime_error("Failed to write deltas");
	}
}

std::vector<Timestamp> ChunkFile::read_deltas(std::ifstream& file)
{
	// Read the number of deltas first
	size_t num_deltas;
	file.read(reinterpret_cast<char*>(&num_deltas), sizeof(num_deltas));
	if (file.fail())
	{
		throw std::runtime_error("Failed to read number of deltas");
	}

	// Then read the delta values into a vector
	std::vector<Timestamp> deltas(num_deltas);
	file.read(reinterpret_cast<char*>(deltas.data()), num_deltas * sizeof(Timestamp));
	if (file.fail())
	{
		throw std::runtime_error("Failed to read deltas");
	}

	return deltas;
}

void ChunkFile::write_values(std::ofstream& file, const std::vector<double>& values)
{
	// Write the number of deltas
	size_t num_values = values.size();
	file.write(reinterpret_cast<const char*>(&num_values), sizeof(num_values));
	if (file.fail())
	{
		throw std::runtime_error("Failed to write number of values");
	}

	// Then write the actual delta
	file.write(reinterpret_cast<const char*>(values.data()), num_values * sizeof(double));
	if (file.fail())
	{
		throw std::runtime_error("Failed to write values");
	}
}

std::vector<double> ChunkFile::read_values(std::ifstream& file)
{
	// Read the number of deltas first
	size_t num_values;
	file.read(reinterpret_cast<char*>(&num_values), sizeof(num_values));
	if (file.fail())
	{
		throw std::runtime_error("Failed to read number of deltas");
	}

	// Then read the delta values into a vector
	std::vector<double> values(num_values);
	file.read(reinterpret_cast<char*>(values.data()), num_values * sizeof(double));
	if (file.fail())
	{
		throw std::runtime_error("Failed to read values");
	}

	return values;
}