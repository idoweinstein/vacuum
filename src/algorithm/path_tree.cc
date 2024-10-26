#include "path_tree.h"

#include <cstddef>

std::size_t PathTree::insertRoot(const Position& position)
{
    node_pool.emplace_back(
        std::nullopt,                  // PathNode.parent_index
        Direction::North,              // PathNode.direction (doesn't matter - it's the first path node)
        position,                      // PathNode.position
        0,                             // PathNode.depth
        0                              // PathNode.score
    );

    visited_positions.insert(position);

    return 0; // Root Node Index
}

std::optional<std::size_t> PathTree::insertChild(std::size_t parent_index, Direction direction_to_child, const Position& child_position, bool is_todo_position)
{
    validateIndex(parent_index);
    PathNode& parent = node_pool.at(parent_index);

    if (visited_positions.contains(child_position))
    {
        return std::nullopt;
    }

    std::size_t depth = parent.depth + 1;
    std::size_t score = parent.score + (is_todo_position ? 1 : 0);

    node_pool.emplace_back(
        parent_index,               // PathNode.parent_index
        direction_to_child,         // PathNode.direction
        child_position,             // PathNode.position
        depth,                      // PathNode.depth
        score                       // PathNode.score
    );

    visited_positions.insert(child_position);

    std::size_t child_index = node_pool.size() - 1;
    return child_index;
}

std::optional<std::size_t> PathTree::getBestEndNodeIndex() const
{
    if (end_node_indices.empty())
    {
        return std::nullopt;
    }

    std::size_t max_score = 0;
    std::size_t best_end_node_index = 0;
    bool found = false;

    for (std::size_t end_node_index : end_node_indices)
    {
        const PathNode& end_node = safeNodeAccess(end_node_index);
        if (end_node.score > max_score || !found)
        {
            max_score = end_node.score;
            best_end_node_index = end_node_index;
            found = true;
        }
    }

    return best_end_node_index;
}