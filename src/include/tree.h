#pragma once

#include "config.h"
#include "utils.h"
#include "chunkfile.h"

#include <cstddef>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>
#include <variant> 


class ChunkTreeNode
{
  public:
	// Each key is a time range
	std::vector<Timestamp> keys;
	// Each value is either another node (for internal nodes) or chunk (for leaves)
	std::vector<std::variant<std::unique_ptr<ChunkTreeNode>, std::shared_ptr<ChunkFile>>> children;
	// Next node in the sequence
	ChunkTreeNode* next_node; // B+ Tree functionality 

	ChunkTreeNode(const bool leaf = false, const size_t node_capacity = Config::MAX_NODE_SIZE)
		: m_node_capacity(node_capacity)
		, m_is_leaf(leaf)
	{
		keys.reserve(node_capacity);
		children.reserve(node_capacity + 1);
		next_node = nullptr;
	}

	bool is_full() const { return children.size() == m_node_capacity; }
	bool is_leaf() const { return m_is_leaf; }

  private:
	size_t m_node_capacity;
	bool m_is_leaf;
};

class ChunkTree
{
  public:
	ChunkTree(const std::string& data_path, const TimeDelta chunk_interval_secs)
		: m_root(std::make_unique<ChunkTreeNode>(true))
		, m_data_path(data_path)
		, m_chunk_interval_secs(chunk_interval_secs)
	{
	}

	std::vector<std::shared_ptr<ChunkFile>> range_query(const TimeRange& range) const;
	void insert(const TimeRange& range, std::shared_ptr<ChunkFile> chunk_file);

  private:
	std::unique_ptr<ChunkTreeNode> m_root;
	std::string m_data_path;
	TimeDelta m_chunk_interval_secs;

	// Utils
	bool in_chunk_range(Timestamp chunk_end_ts, Timestamp timestamp) const
	{
		return timestamp >= chunk_end_ts && timestamp < chunk_end_ts - m_chunk_interval_secs;
	}

	// Querying
	void gather_chunk_files_in_range(
		const TimeRange& range,
		std::vector<std::shared_ptr<ChunkFile>>& results
	) const;

	// Insertion
	void split(ChunkTreeNode* parent, size_t index);
	void insert_non_full(
		ChunkTreeNode* node,
		const TimeRange& range,
		std::shared_ptr<ChunkFile> chunk_file
	);
};