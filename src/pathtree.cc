#include "pathtree.h"

unsigned int PathTree::insertRoot(Position position)
{
    PathNode root_node = {
        kNoParent,          // PathNode.parent_index
        Direction::STAY,    // PathNode.direction (doesn't matter - it's the first path node)
        position            // PathNode.position
    };
    node_pool.push_back(root_node);

    return 0; // Root Index
}

unsigned int PathTree::insertChild(unsigned int parent_index, Direction direction_to_child, Position child_position)
{
    PathNode child_node = {
        (int)parent_index,  // PathNode.parent_index
        direction_to_child, // PathNode.direction
        child_position      // PathNode.position
    };
    node_pool.push_back(child_node);

    unsigned int child_index = node_pool.size() - 1;
    return child_index;
}