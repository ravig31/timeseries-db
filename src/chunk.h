#pragma once

#include "config.h"
#include "datapoint.h"
#include "utils.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

class Chunk
{
  public:
	Chunk(TimeRange range, ChunkId id = 0, size_t capacity = Config::MAX_CHUNK_SIZE)
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

	std::vector<DataPoint> get_data_in_range(const TimeRange& range) const;
	void append(const DataPoint& point);

	ChunkId id() const { return m_id; }
	const TimeRange& get_range() const { return m_range; }
	bool is_to_save() const { return m_is_to_save; }
	size_t size() const { return m_row_count; }
	bool is_full() const { return m_ts_deltas.size() >= m_capacity; }
	void mark_to_save() { m_is_to_save = true; }
	void unmark_to_save() { m_is_to_save = false; }

  private:
	std::vector<Timestamp> m_ts_deltas;
	std::vector<double> m_values;

	const TimeRange m_range;
	const ChunkId m_id;
	const size_t m_capacity;
	size_t m_row_count;
	bool m_is_to_save;

	friend class ChunkFile;
};

class ChunkFile
{
  public:
	struct Metadata
	{
		ChunkId chunk_id;
		TimeRange chunk_range;
		size_t row_count;
	};

	ChunkFile(
		const std::string& base_path,
		ChunkId chunk_id,
		TimeRange chunk_range,
		size_t row_count
	)
		: m_chunk_path(generate_filepath(base_path, chunk_id))
		, m_metadata(Metadata{ chunk_id, chunk_range, row_count })
	{
	}
	void save(const Chunk& chunk) const;
	std::unique_ptr<Chunk> load() const;
	const Metadata& get_metadata() const { return m_metadata; }

  private:
	std::string m_chunk_path;
	const Metadata m_metadata;

	static std::string generate_filepath(const std::string& base_dir, const int64_t chunk_id)
	{
		return base_dir + "/chunk_" + std::to_string(chunk_id) + ".bin";
	}

	static void write_metadata(std::ofstream& file, const Chunk& chunk);
	static Metadata read_metadata(std::ifstream& file);
	static void write_deltas(std::ofstream& file, const std::vector<Timestamp>& deltas);
	static std::vector<Timestamp> read_deltas(std::ifstream& file);
	static void write_values(std::ofstream& file, const std::vector<double>& values);

	static std::vector<double> read_values(std::ifstream& file);
};
