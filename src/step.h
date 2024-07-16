#ifndef VACUUM_STEP_H_
#define VACUUM_STEP_H_

#include <ostream>
#include <string>

/**
 * @brief The Step enum represents the possible suggestions the algorithm can give to the simulator.
 */
enum class Step { North, East, South, West, Stay, Finish };

// Adapting Step enum class to be streamable, for logging purposes:
namespace std
{
    /**
     * @brief Overload of the stream operator for the Step enum.
     */
    inline std::ostream& operator<<(std::ostream& ostream, const Step& step)
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
}

#endif /* VACUUM_STEP_H_ */
