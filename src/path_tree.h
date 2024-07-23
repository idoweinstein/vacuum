#ifndef PATH_TREE_H_
#define PATH_TREE_H_

#include <vector>
#include <stdexcept>
#include <unordered_set>

#include "position.h"
#include "enums.h"

/**
 * @brief Represents a node in the path tree.
 */
struct PathNode
{
    int parent_index;                // Index of the parent node in the path tree.
    bool is_path_end;                // Boolean inidicating whether or not this node is end of a valid path.
    Direction direction;             // Direction from the parent node to this node.
    Position position;               // Position of this node.
    std::size_t depth;               // Depth of this node in the path tree.
    unsigned int accumulated_score;  // Accumulated Score of the path prefix ends at this node.
};

/**
 * @brief Represents a tree structure to store paths.
 */
class PathTree
{
    static constexpr const int kNoParent = -1; // Constant representing no parent index.

    std::vector<PathNode> node_pool;           // Pool of path nodes.
    std::vector<unsigned int> leaf_nodes;

    /**
     * @brief Validates the given node index.
     * @param node_index The index of the node to validate.
     * @throws std::out_of_range if the node index is invalid.
     */
    void validateIndex(unsigned int node_index) const
    {
        if (node_index >= node_pool.size())
        {
            throw std::out_of_range("Given an invalid Path Tree Node index!");
        }
    }

    /**
     * @brief Safely accesses the node at the given index.
     * @param node_index The index of the node to access.
     * @return Reference to the node at the given index.
     * @throws std::out_of_range if the node index is invalid.
     */
    PathNode& safeNodeAccess(unsigned int node_index)
    {
        validateIndex(node_index);
        return node_pool.at(node_index);
    }

public:
    /**
     * @brief Inserts a root node into the path tree.
     * @param position The position of the root node.
     * @return The index of the inserted root node.
     */
    unsigned int insertRoot(Position position);

    /**
     * @brief Inserts a child node into the path tree.
     * @param parent_index The index of the parent node.
     * @param direction_to_child The direction from the parent node to the child node.
     * @param child_position The position of the child node.
     * @return The index of the inserted child node.
     */
    unsigned int insertChild(unsigned int parent_index, Direction direction_to_child, Position child_position, unsigned int child_score);

    /**
     * @brief Gets the index of the parent node for the given node index.
     * @param node_index The index of the node.
     * @return The index of the parent node.
     */
    int getParentIndex(unsigned int node_index) { return safeNodeAccess(node_index).parent_index; }

    /**
     * @brief Checks if the given node has a parent.
     * @param node_index The index of the node.
     * @return True if the node has a parent, false otherwise.
     */
    bool hasParent(unsigned int node_index) { return (kNoParent != getParentIndex(node_index)); }

    /**
     * @brief Gets the position of the node at the given index.
     * @param node_index The index of the node.
     * @return The position of the node.
     */
    Position getPosition(unsigned int node_index) { return safeNodeAccess(node_index).position; }

    /**
     * @brief Gets the direction from the parent node to the node at the given index.
     * @param node_index The index of the node.
     * @return The direction from the parent node to the node.
     */
    Direction getDirection(unsigned int node_index) { return safeNodeAccess(node_index).direction; }

    /**
     * @brief Gets the depth of the node at the given index.
     * @param node_index The index of the node.
     * @return The depth of the node.
     */
    std::size_t getDepth(unsigned int node_index) { return safeNodeAccess(node_index).depth; }

    unsigned int getScore(unsigned int node_index) { return safeNodeAccess(node_index).accumulated_score; }

    void markAsPathEnd(unsigned int node_index)
    {
        leaf_nodes.push_back(node_index);
        safeNodeAccess(node_index).is_path_end = true;
    }

    std::vector<unsigned int>::iterator begin() { return leaf_nodes.begin(); }
    std::vector<unsigned int>::const_iterator cbegin() const { return leaf_nodes.cbegin(); }

    std::vector<unsigned int>::iterator end() { return leaf_nodes.end(); }
    std::vector<unsigned int>::const_iterator cend() const { return leaf_nodes.cend(); }
};

#endif /* PATH_TREE_H_ */
