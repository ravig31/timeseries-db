#pragma once

#include "datapoint.h"
#include "utils.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

class Chunk
{
  public:
	Chunk(TimeRange& timerange, ChunkId id = 0)
		: m_id(id)
		, m_row_count(0)
		, m_is_to_save(false)
		, m_timerange(timerange)
	{
		m_ts_deltas.reserve(MAX_CHUNK_SIZE);
		m_values.reserve(MAX_CHUNK_SIZE);
	};

	void append(const DataPoint& point);
	std::vector<DataPoint> query() const;
	TimeRange get_range() const { return m_timerange; }
	int64_t id() const { return m_id; }
	bool is_to_save() const { return m_is_to_save; }
	size_t size() const { return m_row_count; }
	bool is_full() const { return m_ts_deltas.size() >= MAX_CHUNK_SIZE; }

  private:
	static constexpr size_t MAX_CHUNK_SIZE{ 1024 };
	std::vector<Timestamp> m_ts_deltas;
	std::vector<double> m_values;

	int64_t m_id;
	size_t m_row_count;
	bool m_is_to_save;
	TimeRange m_timerange;

	friend class ChunkFile;
};

class ChunkFile
{
  public:
	ChunkFile(const std::string& base_path, ChunkId chunk_id)
		: m_chunk_path(generate_filepath(base_path, chunk_id))
		, m_chunk_id(chunk_id)
	{
	}
	void save(const Chunk& chunk) const;
	std::unique_ptr<Chunk> load() const;

  private:
	struct ChunkHeader
	{
		TimeRange time_range;
		ChunkId chunk_id;
	};
	std::string m_chunk_path;
	int64_t m_chunk_id;

	static std::string generate_filepath(const std::string& base_dir, int64_t chunk_id)
	{
		return base_dir + "/chunk_" + std::to_string(chunk_id) + ".bin";
	}
	void write_header(std::ofstream& file, const Chunk& chunk) const;
	ChunkHeader read_header(std::ifstream& file) const;
	void write_deltas(std::ofstream& file, const std::vector<Timestamp>& deltas) const;
	std::vector<Timestamp> read_deltas(std::ifstream& file) const;
	void write_values(std::ofstream& file, const std::vector<double>& values) const;
	std::vector<double> read_values(std::ifstream& file) const;
};

class ChunkTreeNode
{
  public:
	// Each key is a time range
	std::vector<TimeRange> keys;
	size_t key_count;
	// Each value is either another node (for internal nodes) or chunk (for leaves)
	std::vector<std::variant<std::unique_ptr<ChunkTreeNode>, std::unique_ptr<ChunkFile>>> children;
	size_t m_node_capacity;
	bool m_is_leaf;
	ChunkTreeNode(bool leaf = false, size_t node_capacity = 128)
		: keys(node_capacity)
		, key_count(0)
		, children(node_capacity)
		, m_node_capacity(node_capacity)
		, m_is_leaf(leaf)
	{
	}

	bool is_full() { return key_count == m_node_capacity; }
	bool is_leaf() { return m_is_leaf; }
};
class ChunkManager
{
  public:
	ChunkManager(const std::string& data_path)
		: m_root(std::make_unique<ChunkTreeNode>(true, MAX_NODE_SIZE))
		, m_data_path(data_path)
	{
	}

	// // Add move operations
	// ChunkManager(ChunkManager&& other) = default;
	// ChunkManager& operator=(ChunkManager&& other) = default;

	// // Delete copy operations
	// ChunkManager(const ChunkManager&) = delete;
	// ChunkManager& operator=(const ChunkManager&) = delete;

	void initialize();
	std::unique_ptr<Chunk> get_chunk(Timestamp timestamp);
	ChunkFile* find_chunk_file(Timestamp ts, std::unique_ptr<ChunkTreeNode>& node);
	void insert_chunk(std::unique_ptr<Chunk> chunk);
	std::unique_ptr<Chunk> create_chunk(Timestamp timestamp, ChunkId id);
	Timestamp get_chunk_key(Timestamp timestamp) { return timestamp - (timestamp % (60 * 60)); }
	ChunkId generate_chunk_id() = delete;

  private:
	std::unique_ptr<ChunkTreeNode> m_root;
	std::string m_data_path;
	static constexpr size_t MAX_NODE_SIZE{ 128 };

	// Cache frequently accessed chunks
	std::unordered_map<Timestamp, std::unique_ptr<Chunk>> chunk_cache;

	void split(ChunkTreeNode* parent, size_t index);
	void insert_non_full(
		ChunkTreeNode* node,
		const TimeRange& range,
		std::unique_ptr<ChunkFile> chunk_file
	);
};