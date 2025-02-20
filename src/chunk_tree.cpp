#include "chunk.h"
#include "config.h"
#include "utils.h"
#include <cstddef>
#include <memory>
#include <vector>

ChunkFile* ChunkTreeNode::find_chunk_file(int64_t ts) const
{

	if (is_leaf())
	{
		for (size_t i = 0; i < keys.size(); ++i)
		{
			if (in_key_range(keys[i], ts))
			{
				// Return the shared_ptr to the ChunkFile
				return std::get<std::unique_ptr<ChunkFile>>(children[i]).get();
			}
		}
		return nullptr;
	}
	else
	{
		for (size_t i = 0; i < keys.size(); ++i)
		{
			if (i == keys.size() - 1 || ts < keys[i + 1])
			{
				auto child_node = std::get<std::unique_ptr<ChunkTreeNode>>(children[i]).get();
				return child_node->find_chunk_file(ts);
			}
		}
		return nullptr;
	}
}

void ChunkTree::insert(Timestamp chunk_start_ts, std::unique_ptr<ChunkFile> chunk_file)
{

	if (m_root->is_full())
	{
		auto old_root = std::move(m_root);
		m_root = std::make_unique<ChunkTreeNode>(false);
		m_root->children.resize(1);
		m_root->children[0] = std::move(old_root);
		split(m_root.get(), 0);
	}
	insert_non_full(m_root.get(), chunk_start_ts, std::move(chunk_file));
}

void ChunkTree::split(ChunkTreeNode* parent, size_t index)

{
	auto child = std::get<std::unique_ptr<ChunkTreeNode>>(parent->children[index]).get();

	auto new_node = std::make_unique<ChunkTreeNode>(child->is_leaf());
	size_t mid_index = (Config::MAX_NODE_SIZE - 1) / 2;

	new_node->keys.insert(
		new_node->keys.end(),
		std::make_move_iterator(child->keys.begin() + mid_index + 1),
		std::make_move_iterator(child->keys.end())
	);

	new_node->children.insert(
		new_node->children.end(),
		std::make_move_iterator(child->children.begin() + mid_index + 1),
		std::make_move_iterator(child->children.end())

	);

	// Update parent
	parent->keys.insert(parent->keys.begin() + index, child->keys[mid_index]);
	parent->children.insert(parent->children.begin() + index + 1, std::move(new_node));

	// Cleanup original child
	child->keys.erase(child->keys.begin() + mid_index, child->keys.end());
	child->children.erase(child->children.begin() + mid_index + 1, child->children.end());
}

void ChunkTree::insert_non_full(
	ChunkTreeNode* node,
	Timestamp chunk_start_ts,
	std::unique_ptr<ChunkFile> chunk_file
)
{
	// TODO: check time complexity of this
	if (node->is_leaf())
	{
		auto chunk_end_ts = chunk_start_ts + Config::CHUNK_INTERVAL_SECS;
		// Find insertion point
		auto it = std::lower_bound(node->keys.begin(), node->keys.end(), chunk_end_ts);
		size_t index = std::distance(node->keys.begin(), it);

		// If can insert keys
		if (!node->is_full())
			// Insert using the insert method for vectors.
			node->keys.insert(it, chunk_end_ts);

		// Find the insertion point in children, which should align with keys.
		auto child_it = node->children.begin() + index;
		node->children.insert(child_it, std::move(chunk_file));
	}
	else
	{
		size_t i = 0;
		while (i < node->keys.size() && chunk_start_ts > node->keys[i])
		{
			i++;
		}
		auto* child = std::get<std::unique_ptr<ChunkTreeNode>>(node->children[i]).get();
		if (child->is_full())
		{
			split(node, i);
			if (chunk_start_ts > node->keys[i])
			{
				i++;
			}
		}
		insert_non_full(
			std::get<std::unique_ptr<ChunkTreeNode>>(node->children[i]).get(),
			chunk_start_ts,
			std::move(chunk_file)
		);
	}
}