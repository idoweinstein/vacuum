#include "path_tree.h"

#include <cstddef>

std::size_t PathTree::insertRoot(const Position& position)
{
    node_pool.emplace_back(
        std::nullopt,       // PathNode.parent_index
        Direction::North,   // PathNode.direction (doesn't matter - it's the first path node)
        position            // PathNode.position
    );

    return 0; // Root Node Index
}

std::size_t PathTree::insertChild(std::size_t parent_index, Direction direction_to_child, const Position& child_position)
{
    validateIndex(parent_index);

    node_pool.emplace_back(
        parent_index,       // PathNode.parent_index
        direction_to_child, // PathNode.direction
        child_position      // PathNode.position
    );

    std::size_t child_index = node_pool.size() - 1;
    return child_index;
}
