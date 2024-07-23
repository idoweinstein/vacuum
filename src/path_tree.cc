#include "path_tree.h"

unsigned int PathTree::insertRoot(Position position)
{
    node_pool.emplace_back(
        kNoParent,          // PathNode.parent_index
        false,              // PathNode.is_path_end
        Direction::North,   // PathNode.direction (doesn't matter - it's the first path node)
        position,           // PathNode.position
        0,                  // PathNode.depth
        0                   // PathNode.accumulated_score
    );

    return 0; // Root Node Index
}

unsigned int PathTree::insertChild(unsigned int parent_index, Direction direction_to_child, Position child_position, unsigned int child_score)
{
    validateIndex(parent_index);
    PathNode& parent_node = node_pool[parent_index];

    node_pool.emplace_back(
        (int)parent_index,                              // PathNode.parent_index
        false,                                          // PathNode.is_path_end
        direction_to_child,                             // PathNode.direction
        child_position,                                 // PathNode.position
        parent_node.depth + 1,                          // PathNode.depth
        parent_node.accumulated_score + child_score     // PathNode.accumulated_score
    );

    unsigned int child_index = node_pool.size() - 1;

    return child_index;
}
