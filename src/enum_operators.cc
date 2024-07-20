#include "enum_operators.h"

#include <string>
#include <ostream>

#include "status.h"
#include "enums.h"

// Adapting Step and Direction enum classes to be streamable, for logging purposes:
namespace std
{
    /**
     * @brief Overload of the stream operator for the Step enum.
     */
    std::ostream& operator<<(std::ostream& ostream, const Step& step)
    {
        std::string string_step;

        switch (step)
        {
            case Step::North:
                string_step = "N";
                break;

            case Step::East:
                string_step = "E";
                break;

            case Step::South:
                string_step = "S";
                break;

            case Step::West:
                string_step = "W";
                break;

            case Step::Stay:
                string_step = "s";
                break;

            case Step::Finish:
                string_step = "F";
                break;
        }

        ostream << string_step;
        return ostream;
    }

    /**
     * @brief Overload of the stream operator for the Direction enum.
     */
    std::ostream& operator<<(std::ostream& ostream, const Direction& direction)
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

    /**
     * @brief Overload of the stream operator for the Status enum.
     */
    std::ostream& operator<<(std::ostream& ostream, const Status& status)
    {
        std::string status_string;

        switch (status)
        {
            case Status::Finished:
                status_string = "FINISHED";
                break;

            case Status::Working:
                status_string = "WORKING";
                break;

            case Status::Dead:
                status_string = "DEAD";
                break;
        }

        ostream << status_string;
        return ostream;
    }
}
