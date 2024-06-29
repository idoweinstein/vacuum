#ifndef VACUUM_POSITION_H_
#define VACUUM_POSITION_H_

#include "direction.h"

#include <functional>
#include <stdexcept>
#include <utility>

/**
 * @class Position
 * @brief Represents a position in a two-dimensional space.
 * @details The Position class is derived from std::pair<int, int> and provides additional functionality for computing positions based on directions.
 */
class Position : public std::pair<int, int> {
public:
    /**
     * @brief Default constructor.
     * @details Initializes the position to (0, 0).
     */
    Position() : std::pair<int, int>(0, 0) {}

    /**
     * @brief Parameterized constructor.
     * @param x The x-coordinate of the position.
     * @param y The y-coordinate of the position.
     */
    Position(int x, int y) : std::pair<int, int>(x, y) {}

    /**
     * @brief Computes a new position based on the current position and given direction.
     * @param position The current position.
     * @param direction The direction to move in.
     * @return The new position after moving in the given direction.
     */
    static Position computePosition(Position position, Direction direction)
    {
        static std::unordered_map<Direction, std::pair<int, int>> direction_map = {
            {Direction::NORTH, std::make_pair(-1, 0)},
            {Direction::SOUTH, std::make_pair(1, 0)},
            {Direction::WEST, std::make_pair(0, -1)},
            {Direction::EAST, std::make_pair(0, 1)},
            {Direction::STAY, std::make_pair(0, 0)}
        };
        return Position(position.first + direction_map[direction].first, position.second + direction_map[direction].second);
    }
};

namespace std {
    /* Source: http://szudzik.com/ElegantPairing.pdf */
    static inline unsigned int elegantPair(unsigned int a, unsigned int b) {
        return a >= b ? a * a + a + b : a + b * b;
    }

    /**
     * @brief Hash function specialization for Position.
     */
    template<> struct hash<Position> {
        /**
         * @brief Computes the hash value for a Position object.
         */
        size_t operator()(Position const& p) const {
            /* Map each integer to a unique unsigned integer */
            unsigned int a = p.first >= 0 ? 2 * p.first : -2 * p.first - 1;
            unsigned int b = p.second >= 0 ? 2 * p.second : -2 * p.second - 1;

            return size_t(elegantPair(a, b));
        }
    };
};

#endif /* VACUUM_POSITION_H_ */
