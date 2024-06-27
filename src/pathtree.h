#ifndef VACUUM_PATHTREE_H_
#define VACUUM_PATHTREE_H_

#include <vector>

#include "position.h"
#include "direction.h"

struct PathNode
{
    int parent_index;
    Direction direction;
    Position position;
};

class PathTree
{
    std::vector<PathNode> node_pool;
    const int kNoParent = -1;

public:
    unsigned int insertRoot(Position position);
    unsigned int insertChild(unsigned int parent_index, Direction direction_to_child, Position child_position);
    int getParentIndex(unsigned int node_index) { return node_pool[node_index].parent_index; }
    bool hasParent(unsigned int node_index) { return (kNoParent == getParentIndex(node_index)); }
    Position getPosition(unsigned int node_index) { return node_pool[node_index].position; }
    Direction getDirection(unsigned int node_index) { return node_pool[node_index].direction; }
};

#endif /* VACUUM_PATHTREE_H_ */