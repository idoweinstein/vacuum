#include "pathtree.h"

unsigned int PathTree::insertRoot(Position position)
{
    node_pool.emplace_back({
        kNoParent,          // PathNode.parent_index
        Direction::STAY,    // PathNode.direction (doesn't matter - it's the first path node)
        position            // PathNode.position
    });

    return 0; // Root Node Index
}

unsigned int PathTree::insertChild(unsigned int parent_index, Direction direction_to_child, Position child_position)
{
    node_pool.emplace_back({
        (int)parent_index,  // PathNode.parent_index
        direction_to_child, // PathNode.direction
        child_position      // PathNode.position
    });

    unsigned int child_index = node_pool.size() - 1;
    return child_index;
}