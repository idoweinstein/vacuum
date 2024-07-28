#ifndef PATH_TREE_H_
#define PATH_TREE_H_

#include <vector>
#include <cstddef>
#include <optional>
#include <stdexcept>

#include "position.h"
#include "enums.h"

/**
 * @brief Represents a path tree data structure.
 */
class PathTree
{
    /**
    * @brief Represents a node in the path tree.
    */
    struct PathNode
    {
        std::optional<std::size_t> parent_index;    // Index of the parent node in the path tree.
        Direction direction;                        // Direction from the parent node to this node.
        Position position;                          // Position of this node.
    };

    std::vector<PathNode> node_pool;                // Pool of path nodes.

    /**
     * @brief Validates the given node index.
     * 
     * @param node_index The index of the node to validate.
     * @throws std::out_of_range if the node index is invalid.
     */
    void validateIndex(std::size_t node_index) const
    {
        if (node_index >= node_pool.size())
        {
            throw std::out_of_range("Given an invalid Path Tree Node index!");
        }
    }

    /**
     * @brief Safely accesses the node at the given index.
     * 
     * @param node_index The index of the node to access.
     * @return Reference to the node at the given index.
     * @throws std::out_of_range if the node index is invalid.
     */
    const PathNode& safeNodeAccess(std::size_t node_index) const
    {
        validateIndex(node_index);
        return node_pool.at(node_index);
    }

public:
    /**
     * @brief Inserts a root node into the path tree.
     * 
     * @param position The position of the root node.
     * @return The index of the inserted root node.
     */
    std::size_t insertRoot(const Position& position);

    /**
     * @brief Inserts a child node into the path tree.
     * 
     * @param parent_index The index of the parent node.
     * @param direction_to_child The direction from the parent node to the child node.
     * @param child_position The position of the child node.
     * @return The index of the inserted child node.
     */
    std::size_t insertChild(std::size_t parent_index, Direction direction_to_child, const Position& child_position);

    /**
     * @brief Gets the index of the parent node for the given node index.
     * 
     * @param node_index The index of the node.
     * @return The index of the parent node.
     */
    std::size_t getParentIndex(std::size_t node_index) const { return safeNodeAccess(node_index).parent_index.value(); }

    /**
     * @brief Checks if the given node has a parent.
     * 
     * @param node_index The index of the node.
     * @return True if the node has a parent, false otherwise.
     */
    bool hasParent(std::size_t node_index) const { return safeNodeAccess(node_index).parent_index.has_value(); }

    /**
     * @brief Gets the position of the node at the given index.
     * 
     * @param node_index The index of the node.
     * @return The position of the node.
     */
    Position getPosition(std::size_t node_index) const { return safeNodeAccess(node_index).position; }

    /**
     * @brief Gets the direction from the parent node to the node at the given index.
     * 
     * @param node_index The index of the node.
     * @return The direction from the parent node to the node.
     */
    Direction getDirection(std::size_t node_index) const { return safeNodeAccess(node_index).direction; }
};

#endif /* PATH_TREE_H_ */
