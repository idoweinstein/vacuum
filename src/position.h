#ifndef POSITION_H_
#define POSITION_H_

#include <utility>
#include <stdexcept>
#include <functional>

#include "enums.h"

/**
 * @class Position
 * @brief Represents a position in a two-dimensional space.
 * @details The Position class is derived from std::pair<int, int> and provides additional functionality for computing positions based on directions.
 */
class Position : public std::pair<int, int>
{
        inline static const std::unordered_map<Direction, std::pair<int, int>> direction_map = {
            {Direction::North, std::make_pair(-1, 0)},
            {Direction::South, std::make_pair(1, 0)},
            {Direction::West, std::make_pair(0, -1)},
            {Direction::East, std::make_pair(0, 1)},
        };

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
        static Position computePosition(const Position& position, Direction direction)
        {
            std::pair<int, int> shift_direction = direction_map.at(direction);
            return Position(position.first + shift_direction.first, position.second + shift_direction.second);
        }
};

namespace std
{
    /**
     * @brief Computes an elegant pairing of two integers.
     *
     * This function computes a unique unsigned integer for a pair of integers (a, b).
     * The function satisifies the following properties:
     * - It is injective (one-to-one).
     * - It tends to yield smaller values for smaller integers.
     * - It favors grid-locality - pairs with the same first/second element tend to
     *  have similar values.
     *
     * Source: http://szudzik.com/ElegantPairing.pdf
     *
     * @param a The first integer.
     * @param b The second integer.
     * @return The unique unsigned integer representing the pair (a, b).
     */
    static inline unsigned int elegantPair(unsigned int a, unsigned int b)
    {
        return a >= b ? a * a + a + b : a + b * b;
    }

    /**
     * @brief Hash function specialization for Position.
     */
    template<> struct hash<Position>
    {
        /**
         * @brief Computes the hash value for a Position object.
         */
        size_t operator()(Position const& position) const
        {
            // Map each integer to a unique unsigned integer
            unsigned int mapped_first = position.first >= 0 ? 2 * position.first : -2 * position.first - 1;
            unsigned int mapped_second = position.second >= 0 ? 2 * position.second : -2 * position.second - 1;

            return size_t(elegantPair(mapped_first, mapped_second));
        }
    };
};

#endif /* POSITION_H_ */
