#ifndef PATH_TREE_H_
#define PATH_TREE_H_

#include <vector>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <unordered_set>

#include "common/position.h"
#include "common/enums.h"

/**
 * @brief Represents a path tree data structure.
 *
 * The PathTree class represents a path tree data structure that stores paths explored during navigation.
 * The root node is the starting position, and the end nodes are the positions that satisfy the found criteria.
 * Branches of the tree represent possible paths to the end nodes.
 *
 * The path tree is not a BFS tree, since position can be visited multiple times in different branches.
 * However, a position can be visited only once in the same branch.
 *
 * Each node contains the aggregated score of the path from the root node to the current node.
 * The score is calculated based on the number of todo positions visited in the path.
 * This scoring method encourages the algorithm to opportunistically visit more todo positions in the path.
 */
class PathTree
{
    /**
    * @brief Represents a node in the path tree.
    */
    struct PathNode
    {
        std::optional<std::size_t> parent_index;        // Index of the parent node in the path tree.
        Direction direction;                            // Direction from the parent node to this node.
        Position position;                              // Position of this node.
        std::size_t depth;                              // Distance between this node and the root node.
        std::size_t score;                              // Score of the node.
    };

    std::vector<PathNode> node_pool;                // Pool of path nodes.
    std::vector<std::size_t> end_node_indices;      // Indices of the end nodes in the path tree.
    std::unordered_set<Position> visited_positions; // Visited positions in tree

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
     * @brief Inserts a child node into the path tree if its position doesn't exist in the current branch.
     * 
     * @param parent_index The index of the parent node.
     * @param direction_to_child The direction from the parent node to the child node.
     * @param child_position The position of the child node.
     * @param is_todo_position True if the child position is a todo position, false otherwise.
     * @return The index of the inserted child node if inserted.
     */
    std::optional<std::size_t> insertChild(std::size_t parent_index, Direction direction_to_child, const Position& child_position, bool is_todo_position);

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

    /**
     * @brief Gets the depth of the node at the given index.
     * 
     * @param node_index The index of the node.
     * @return The depth of the node.
     */
    std::size_t getDepth(std::size_t node_index) const { return safeNodeAccess(node_index).depth; }

    /**
     * @brief Gets the score of the node at the given index.
     * 
     * @param node_index The index of the node.
     * @return The score of the node.
     */
    std::size_t getScore(std::size_t node_index) const { return safeNodeAccess(node_index).score; }

    /**
     * @brief Registers an end node index in the path tree.
     * 
     * @param end_node_index The index of the node to be registered as an end node.
     */
    void registerEndNode(std::size_t end_node_index)
    {
        validateIndex(end_node_index);
        end_node_indices.push_back(end_node_index);
    }

    /**
     * @brief Gets the index of the end node with the highest score.
     * 
     * @return The index of the end node with the highest score if exists.
     */
    std::optional<std::size_t> getBestEndNodeIndex() const;
};

#endif /* PATH_TREE_H_ */
