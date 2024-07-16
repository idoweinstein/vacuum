#include "pathtree.h"

unsigned int PathTree::insertRoot(Position position)
{
    node_pool.emplace_back(
        kNoParent,          // PathNode.parent_index
        Direction::North,   // PathNode.direction (doesn't matter - it's the first path node)
        position,           // PathNode.position
        0                   // PathNode.depth
    );

    return 0; // Root Node Index
}

unsigned int PathTree::insertChild(unsigned int parent_index, Direction direction_to_child, Position child_position)
{
    validateIndex(parent_index);

    node_pool.emplace_back(
        (int)parent_index,                // PathNode.parent_index
        direction_to_child,               // PathNode.direction
        child_position,                   // PathNode.position
        node_pool[parent_index].depth + 1 // PathNode.depth
    );

    unsigned int child_index = node_pool.size() - 1;
    return child_index;
}
