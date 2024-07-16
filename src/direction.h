#ifndef VACUUM_DIRECTION_H_
#define VACUUM_DIRECTION_H_

#include "step.h"

#include <ostream>
#include <string>

/**
 * @brief The Direction enum represents the possible directions for the vacuum cleaner.
 */
enum class Direction{ North, East, South, West };

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
            case Direction::North:
                string_direction = "North";
                break;

            case Direction::East:
                string_direction = "East";
                break;

            case Direction::South:
                string_direction = "South";
                break;

            case Direction::West:
                string_direction = "West";
                break;

        }

        ostream << string_direction;
        return ostream;
    }
}

#endif /* VACUUM_DIRECTION_H_ */
