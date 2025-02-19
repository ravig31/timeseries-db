#include "chunk.h"
#include "utils.h"
#include <cstddef>
#include <fstream>
#include <memory>
#include <vector>

void ChunkFile::save(const Chunk& chunk) const
{
	std::ofstream outf(m_chunk_path, std::ios::binary);
	if (!outf.is_open())
	{
		throw std::runtime_error("Failed to open chunk file for saving: " + m_chunk_path);
	}

	try
	{
		write_header(outf, chunk);
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

	ChunkHeader header;
	std::vector<Timestamp> deltas;
	std::vector<double> values;

	try
	{
		header = read_header(inf);
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
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(header.time_range, header.chunk_id);

	return chunk;
}

void ChunkFile::write_header(std::ofstream& file, const Chunk& chunk) const
{
	ChunkHeader header{ chunk.m_timerange, chunk.id() };
	file.write(reinterpret_cast<const char*>(&header), sizeof(header));
	if (file.fail())
	{
		throw std::runtime_error("Failed to write chunk header");
	}
}

ChunkFile::ChunkHeader ChunkFile::read_header(std::ifstream& file) const
{
	ChunkHeader header;

	file.read(reinterpret_cast<char*>(&header), sizeof(header));

	if (file.fail())
	{
		throw std::runtime_error("Failed to read chunk header");
	}

	return header;
}

void ChunkFile::write_deltas(std::ofstream& file, const std::vector<Timestamp>& deltas) const
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

std::vector<Timestamp> ChunkFile::read_deltas(std::ifstream& file) const
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

void ChunkFile::write_values(std::ofstream& file, const std::vector<double>& values) const
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

std::vector<double> ChunkFile::read_values(std::ifstream& file) const
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