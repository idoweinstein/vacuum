#ifndef VACUUM_DIRECTION_H_
#define VACUUM_DIRECTION_H_

#include <ostream>
#include <string>

/**
 * @brief The Direction enum represents the possible directions for the vacuum cleaner.
 */
enum class Direction{ NORTH, EAST, SOUTH, WEST, STAY, FINISH };

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

            case Direction::STAY:
                string_direction = "Stay";
                break;

            case Direction::FINISH:
                string_direction = "Finish";
                break;
        }

        ostream << string_direction;
        return ostream;
    }
}

#endif /* VACUUM_DIRECTION_H_ */
