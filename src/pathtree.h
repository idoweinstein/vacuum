#ifndef VACUUM_PATHTREE_H_
#define VACUUM_PATHTREE_H_

#include <vector>
#include <stdexcept>

#include "direction.h"
#include "position.h"

struct PathNode
{
    int parent_index;
    Direction direction;
    Position position;
};

class PathTree
{
    static constexpr const int kNoParent = -1;

    std::vector<PathNode> node_pool;

    void validateIndex(unsigned int node_index) const
    {
        if (node_index >= node_pool.size())
        {
            throw std::out_of_range("Given an invalid Path Tree Node index!");
        }
    }

    PathNode& safeNodeAccess(unsigned int node_index)
    {
        validateIndex(node_index);
        return node_pool.at(node_index);
    }

public:
    unsigned int insertRoot(Position position);
    unsigned int insertChild(unsigned int parent_index, Direction direction_to_child, Position child_position);
    int getParentIndex(unsigned int node_index) { return safeNodeAccess(node_index).parent_index; }
    bool hasParent(unsigned int node_index) { return (kNoParent != getParentIndex(node_index)); }
    Position getPosition(unsigned int node_index) { return safeNodeAccess(node_index).position; }
    Direction getDirection(unsigned int node_index) { return safeNodeAccess(node_index).direction; }
};

#endif /* VACUUM_PATHTREE_H_ */