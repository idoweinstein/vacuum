#ifndef VACUUM_DIRECTION_H_
#define VACUUM_DIRECTION_H_

#include "step.h"

#include <ostream>
#include <string>

/**
 * @brief The Direction enum represents the possible directions for the vacuum cleaner.
 */
enum class Direction{ NORTH, EAST, SOUTH, WEST, };

inline Step directionToStep(Direction direction)
{
    switch (direction)
    {
        case Direction::NORTH:
            return Step::NORTH;

        case Direction::EAST:
            return Step::EAST;

        case Direction::SOUTH:
            return Step::SOUTH;

        case Direction::WEST:
            return Step::WEST;
        
        default:
            throw std::invalid_argument("Invalid direction");
    }
}

inline Direction stepToDirection(Step step)
{
    switch (step)
    {
        case Step::NORTH:
            return Direction::NORTH;

        case Step::EAST:
            return Direction::EAST;

        case Step::SOUTH:
            return Direction::SOUTH;

        case Step::WEST:
            return Direction::WEST;

        default:
            throw std::invalid_argument("Invalid step");
    }
}

// Adapting Direction enum class to be streamable, for logging purposes:
namespace std
{
    /**
     * @brief Overload of the stream operator for the Direction enum.
     */
    inline std::ostream& operator<<(std::ostream& ostream, const Direction& direction)
    {
        std::string string_direction;

        switch (direction)
        {
            case Direction::NORTH:
                string_direction = "North";
                break;

            case Direction::EAST:
                string_direction = "East";
                break;

            case Direction::SOUTH:
                string_direction = "South";
                break;

            case Direction::WEST:
                string_direction = "West";
                break;

        }

        ostream << string_direction;
        return ostream;
    }
}

#endif /* VACUUM_DIRECTION_H_ */
