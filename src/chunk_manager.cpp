#include "chunk.h"
#include "utils.h"
#include <cstddef>
#include <memory>
#include <random>
#include <vector>



std::unique_ptr<Chunk> ChunkManager::get_chunk(Timestamp ts)
{
	// TODO: Implement cache with invalidation
	// (e.g., using a map or unordered_map with timestamps as keys)

	ChunkFile* chunk_file = find_chunk_file(ts, std::move(m_root));
	if (!chunk_file)
	{
		return nullptr;
	}

	std::unique_ptr<Chunk> chunk = chunk_file->load();
	if (!chunk)
	{
		// Handle load error (e.g., return nullptr or throw an exception)
	}

	// TODO: Add to cache with appropriate invalidation logic

	return chunk;
}

ChunkFile* ChunkManager::find_chunk_file(Timestamp ts, std::unique_ptr<ChunkTreeNode> node)
{
	if (node->is_leaf)
	{
		// Consider using binary search here
		for (size_t i = 0; i < node->keys.size(); ++i)
		{
			if (node->keys[i].contains(ts))
			{
				return std::get<std::unique_ptr<ChunkFile>>(node->children[i]).get();
			}
		}
		return nullptr;
	}
	else
	{
		for (size_t i = 0; i < node->keys.size(); ++i)
		{
			if (i == node->keys.size() - 1 || ts < node->keys[i + 1].start_ts)
			{
				return find_chunk_file(
					ts,
					std::move(std::get<std::unique_ptr<ChunkTreeNode>>(node->children[i]))
				);
			}
		}
		return nullptr;
	}
}

void ChunkManager::insert_chunk(std::unique_ptr<Chunk> chunk)
{

	if (m_root->keys.size() == MAX_NODE_SIZE)
	{
		auto new_root = std::make_unique<ChunkTreeNode>(false);
		new_root->children.push_back(std::move(m_root));
		split(new_root.get(), 0);
		m_root = std::move(new_root);
	}

	auto chunk_file = std::make_unique<ChunkFile>(m_data_path, chunk->id());
	chunk_file->save(*chunk);

	insert_non_full(m_root.get(), chunk->get_range(), std::move(chunk_file));
}

std::unique_ptr<Chunk> ChunkManager::create_chunk(Timestamp timestamp)
{
	Timestamp start_key = get_chunk_key(timestamp);
	TimeRange range{ start_key, start_key + (60 * 60) };
	auto chunk = std::make_unique<Chunk>(generate_chunk_id(), range);
	return chunk;
}

ChunkId ChunkManager::generate_chunk_id()
{
	static std::random_device rd;
	static std::mt19937_64 generator(rd());
	static std::uniform_int_distribution<ChunkId> distribution;
	return distribution(generator);
}

void ChunkManager::split(ChunkTreeNode* parent, size_t index)
{
	auto child = std::get<std::unique_ptr<ChunkTreeNode>>(parent->children[index]).get();
	auto new_node = std::make_unique<ChunkTreeNode>(child->is_leaf);

	size_t mid_index = (MAX_NODE_SIZE - 1) / 2;
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

void ChunkManager::insert_non_full(
	ChunkTreeNode* node,
	const TimeRange& range,
	std::unique_ptr<ChunkFile> chunk_file
)
{
	size_t i{ node->keys.size() - 1 };

	if (node->is_leaf)
	{
		while (i >= 0 && range.start_ts < node->keys[i].start_ts)
		{
			std::swap(node->keys[i + 1], node->keys[i]);
			node->children[i + 1] = std::move(node->children[i]);
			--i;
		}
		node->keys[i + 1] = range;
		node->children[i + 1] = std::move(chunk_file);
	}
	else
	{
		while (i >= 0 && range.start_ts < node->keys[i].start_ts)
		{
			--i;
		}
		++i; // incremenet one for position of right child
		auto child = std::get<std::unique_ptr<ChunkTreeNode>>(node->children[i]).get();
		if (child->keys.size() == MAX_NODE_SIZE)
		{
			split(node, i);
			if (range.start_ts > node->keys[i].start_ts)
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