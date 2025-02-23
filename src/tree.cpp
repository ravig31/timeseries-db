#include "tree.h"
#include "chunk.h"
#include "utils.h"

#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>

std::vector<ChunkFile*> ChunkTree::range_query(const TimeRange& range) const
{
	std::vector<ChunkFile*> results{};
	results.reserve(range.duration() / m_chunk_interval_secs);
	gather_chunk_files_in_range(m_root.get(), range, results);

	return results;
}

void ChunkTree::insert(const TimeRange& range, std::unique_ptr<ChunkFile> chunk_file)
{

	if (m_root->is_full())
	{
		auto old_root = std::move(m_root);
		m_root = std::make_unique<ChunkTreeNode>(false);
		m_root->children.resize(1);
		m_root->children[0] = std::move(old_root);
		split(m_root.get(), 0);
	}
	insert_non_full(m_root.get(), range, std::move(chunk_file));
}

ChunkFile* ChunkTree::find_chunk_file(ChunkTreeNode* node, Timestamp ts) const
{

	if (node->is_leaf())
	{
		for (size_t i = 0; i < node->keys.size(); ++i)
		{
			if (in_chunk_range(node->keys[i], ts))
			{
				// Return the shared_ptr to the ChunkFile
				return std::get<std::unique_ptr<ChunkFile>>(node->children[i]).get();
			}
		}
		return nullptr;
	}
	else
	{
		for (size_t i = 0; i < node->keys.size(); ++i)
		{
			if (i == node->keys.size() - 1 || ts < node->keys[i + 1])
			{
				auto child_node = std::get<std::unique_ptr<ChunkTreeNode>>(node->children[i]).get();
				return find_chunk_file(child_node, ts);
			}
		}
		return nullptr;
	}
}

void ChunkTree::gather_chunk_files_in_range(
	ChunkTreeNode* node,
	const TimeRange& range,
	std::vector<ChunkFile*>& results
) const
{
	if (!node)
		return;

	if (node->is_leaf())
	{
		for (size_t i{ 0 }; i < node->keys.size(); i++)
		{
			if (range.contains(node->keys[i]))
			{
				results.push_back(std::get<std::unique_ptr<ChunkFile>>(node->children[i]).get());
			}
		}
	}
}

void ChunkTree::split(ChunkTreeNode* parent, size_t index)

{
	auto child = std::get<std::unique_ptr<ChunkTreeNode>>(parent->children[index]).get();
	size_t mid_index = (child->children.size() - 1) / 2;

	auto new_node = std::make_unique<ChunkTreeNode>(child->is_leaf());
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
	const TimeRange& range,
	std::unique_ptr<ChunkFile> chunk_file
)
{

	// TODO: check time complexity of this
	if (node->is_leaf())
	{
		assert(node != nullptr && "node should not be null in insert_non_full"); // Add this!
		// Find insertion point
		size_t index{ 0 };
		if (!node->keys.empty())
		{
			auto it = std::lower_bound(node->keys.begin(), node->keys.end(), range.end_ts);
			index = std::distance(node->keys.begin(), it);
		}

		// If can insert keys
		if (!node->is_full())
			// Insert using the insert method for vectors.
			node->keys.insert(node->keys.begin() + index, range.end_ts);

		// Find the insertion point in children, which should align with keys.
		auto child_it = node->children.begin() + index;
		node->children.insert(child_it, std::move(chunk_file));
	}
	else
	{
		size_t i = 0;
		while (i < node->keys.size() && range.start_ts > node->keys[i])
		{
			i++;
		}
		auto* child = std::get<std::unique_ptr<ChunkTreeNode>>(node->children[i]).get();
		if (child->is_full())
		{
			split(node, i);
			if (range.start_ts > node->keys[i])
			{
				i++;
			}
		}
		insert_non_full(
			std::get<std::unique_ptr<ChunkTreeNode>>(node->children[i]).get(),
			range,
			std::move(chunk_file)
		);
	}
}
