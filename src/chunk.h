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
	Chunk(Timestamp start_ts, ChunkId id = 0)
		: m_id(id)
		, m_points(0)
		, m_is_to_save(false)
		, m_start_ts(start_ts)
	{
		m_ts_deltas.reserve(Config::MAX_CHUNK_SIZE);
		m_values.reserve(Config::MAX_CHUNK_SIZE);
	};

	void append(const DataPoint& point);
	std::vector<DataPoint> query() const;
	int64_t id() const { return m_id; }
	bool in_time_range(Timestamp ts) const
	{
		return ts >= m_start_ts && ts <= m_start_ts + Config::CHUNK_INTERVAL_SECS;
	}
	Timestamp get_start_ts() const { return m_start_ts; }
	bool is_to_save() const { return m_is_to_save; }
	size_t size() const { return m_points; }
	bool is_full() const { return m_ts_deltas.size() >= Config::MAX_CHUNK_SIZE; }

  private:
	std::vector<Timestamp> m_ts_deltas;
	std::vector<double> m_values;

	int64_t m_id;
	size_t m_points;
	bool m_is_to_save;
	Timestamp m_start_ts;

	friend class ChunkFile;
};

class ChunkFile
{
  public:
	ChunkFile(const std::string& base_path, ChunkId chunk_id, Timestamp chunk_start_ts)
		: m_chunk_path(generate_filepath(base_path, chunk_id))
		, m_chunk_id(chunk_id)
		, m_chunk_start_ts(chunk_start_ts)
	{
	}
	void save(const Chunk& chunk) const;
	std::unique_ptr<Chunk> load() const;

  private:
	struct ChunkHeader
	{
		Timestamp chunk_start_ts;
		ChunkId chunk_id;
	};
	std::string m_chunk_path;
	int64_t m_chunk_id;
	Timestamp m_chunk_start_ts;

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
	std::vector<Timestamp> keys;
	size_t key_count;
	// Each value is either another node (for internal nodes) or chunk (for leaves)
	std::vector<std::variant<std::unique_ptr<ChunkTreeNode>, std::unique_ptr<ChunkFile>>> children;
	size_t m_node_capacity;
	bool m_is_leaf;
	ChunkTreeNode(bool leaf = false, size_t node_capacity = Config::MAX_NODE_SIZE)
		: key_count(0)
		, m_node_capacity(node_capacity)
		, m_is_leaf(leaf)
	{
		keys.reserve(node_capacity);
		keys.reserve(node_capacity + 1);
	}

	bool is_full() const { return children.size() == m_node_capacity; }
	bool is_leaf() const { return m_is_leaf; }
	ChunkFile* find_chunk_file(int64_t timestamp) const;
	static bool in_key_range(Timestamp start_ts, Timestamp timestamp)
	{
		return timestamp >= start_ts && timestamp < start_ts + Config::CHUNK_INTERVAL_SECS;
	}
};

class ChunkTree
{
  public:
	ChunkTree(const std::string& data_path)
		: m_root(std::make_unique<ChunkTreeNode>(true))
		, m_data_path(data_path)
	{
	}

	// // Add move operations
	// ChunkTree(ChunkTree&& other) = default;
	// ChunkTree& operator=(ChunkTree&& other) = default;

	// // Delete copy operations
	// ChunkTree(const ChunkTree&) = delete;
	// ChunkTree& operator=(const ChunkTree&) = delete;

	void insert(Timestamp chunk_start_ts, std::unique_ptr<ChunkFile> chunk_file);
	ChunkId generate_chunk_id() = delete;
	ChunkFile* get_chunk_file(int64_t ts) const { return m_root->find_chunk_file(ts); }

  private:
	std::unique_ptr<ChunkTreeNode> m_root;
	std::string m_data_path;

	// Cache frequently accessed chunks

	void split(ChunkTreeNode* parent, size_t index);
	void insert_non_full(
		ChunkTreeNode* node,
		const Timestamp chunk_start_ts,
		std::unique_ptr<ChunkFile> chunk_file
	);

	friend class ChunkTreeNode;
};