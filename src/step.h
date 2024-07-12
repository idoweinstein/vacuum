#ifndef VACUUM_STEP_H_
#define VACUUM_STEP_H_

#include <ostream>
#include <string>

/**
 * @brief The Step enum represents the possible suggestions the algorithm can give to the simulator.
 */
enum class Step { NORTH, EAST, SOUTH, WEST, STAY, FINISH };

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
            case Step::NORTH:
                string_step = "North";
                break;

            case Step::EAST:
                string_step = "East";
                break;

            case Step::SOUTH:
                string_step = "South";
                break;

            case Step::WEST:
                string_step = "West";
                break;

            case Step::STAY:
                string_step = "Stay";
                break;

            case Step::FINISH:
                string_step = "Finish";
                break;
        }

        ostream << string_step;
        return ostream;
    }
}

#endif /* VACUUM_STEP_H_ */
