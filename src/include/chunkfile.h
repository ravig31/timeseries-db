#pragma once

#include <string>
#include <vector>

#include "utils.h"
#include "chunkfilemetadata.h"

class Chunk;

class ChunkFile{
  public:
	ChunkFile(
		const std::string& base_path,
		ChunkId chunk_id,
		TimeRange chunk_range,
		size_t row_count,
		size_t capacity
	)
		: m_chunk_path(generate_filepath(base_path, chunk_id))
		, m_metadata(ChunkMetadata{ chunk_id, chunk_range, row_count, capacity })
	{
	}
	void save(const Chunk& chunk) const;
	std::unique_ptr<Chunk> load() const;
	const ChunkMetadata& get_metadata() const { return m_metadata; }

  private:
	std::string m_chunk_path;
	const ChunkMetadata m_metadata;

	static std::string generate_filepath(const std::string& base_dir, const int64_t chunk_id)
	{
		return base_dir + "/chunk_" + std::to_string(chunk_id) + ".bin";
	}

	static void write_metadata(std::ofstream& file, const Chunk& chunk);
	static ChunkMetadata read_metadata(std::ifstream& file);
	static void write_deltas(std::ofstream& file, const std::vector<Timestamp>& deltas);
	static std::vector<Timestamp> read_deltas(std::ifstream& file);
	static void write_values(std::ofstream& file, const std::vector<double>& values);

	static std::vector<double> read_values(std::ifstream& file);
};
