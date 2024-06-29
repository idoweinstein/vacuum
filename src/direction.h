#ifndef VACUUM_DIRECTION_H_
#define VACUUM_DIRECTION_H_

#include <format>
#include <string>

/**
 * @brief The Direction enum represents the possible directions for the vacuum cleaner.
 */
enum class Direction{ NORTH, EAST, SOUTH, WEST, STAY };

// Adapting Direction enum to be formatted in the Logger:
namespace std
{
    /**
     * @brief Specialization of the std::formatter for the Direction enum.
     */
    template <>
    struct formatter<Direction> : formatter<string>
    {
        /**
         * @brief Formats the Direction enum value as a string.
         */
        auto format(const Direction direction, format_context& ctx) const
        {
            string string_direction;

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
            }

            return formatter<string>::format(string_direction, ctx);
        }
    };
}

#endif /* VACUUM_DIRECTION_H_ */
