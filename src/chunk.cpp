#include "chunk.h"
#include "utils.h"
#include <cstddef>
#include <fstream>
#include <memory>
#include <vector>

void Chunk::append(const DataPoint& point)
{
	if (is_full())
	{
		m_is_to_save = true;
		return;
	}

	int64_t timedelta = point.encode_time_delta(m_range.start_ts);
	m_ts_deltas.push_back(timedelta);
	m_values.push_back(point.value);
	m_points++;
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

	ChunkFile::Metadata metadata;
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

	inf.close();

	// Create and return a Chunk object
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(metadata.chunk_range, metadata.chunk_id);

	return chunk;
}

void ChunkFile::write_metadata(std::ofstream& file, const Chunk& chunk)
{
	ChunkFile::Metadata metadata{ chunk.id(), chunk.get_range() };
	file.write(reinterpret_cast<const char*>(&metadata), sizeof(metadata));
	if (file.fail())
	{
		throw std::runtime_error("Failed to write chunk metadata");
	}
}

ChunkFile::ChunkFile::Metadata ChunkFile::read_metadata(std::ifstream& file)
{
	ChunkFile::Metadata metadata;

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