#include "chunk.h"
#include "utils.h"
#include <cstddef>
#include <fstream>
#include <memory>
#include <vector>



void ChunkFile::save(const Chunk& chunk) const
{
	std::ofstream outf(m_chunk_path, std::ios::binary | std::ios::trunc);
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

std::unique_ptr<Chunk> ChunkFile::load()
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
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(header.chunk_id, header.time_range);

	return chunk;
}

