#ifndef ROBOT_SIMULATOR_H_
#define ROBOT_SIMULATOR_H_

#include <vector>
#include <string>
#include <memory>

#include "abstract_algorithm.h"
#include "position.h"
#include "battery.h"
#include "status.h"
#include "house.h"
#include "enums.h"

/**
 * @brief The Simulator class represents a vacuum cleaning robot.
 *
 * This class encapsulates the main functionality of a vacuum cleaning robot.
 */
class Simulator
{
    /**
     * @brief Enum describing the different states of the simulator initialization
     */
    enum class SimulatorState
    {
        Initial,
        Deserialized,
        Ready
    };

    SimulatorState state = SimulatorState::Initial;     // Simulator's initialization current state.
    unsigned int max_simulator_steps = 0;               // Maximum number of steps the simulator can perform.
    std::unique_ptr<Battery> battery = nullptr;         // Simulator's battery (for charging / discharging and getting battery level).
    std::unique_ptr<House> house = nullptr;             // Simulator's house representation.
    AbstractAlgorithm* algorithm = nullptr;             // Simulator's algorithm to suggest its next steps.

    /**
     * @brief Checks if the cleaning mission is complete.
     *
     * @return true if the cleaning mission is complete, false otherwise.
     */
    bool isMissionComplete() const { return (house->isFinished() && house->isInDockingStation()); }

    static Status getMissionStatus(bool is_algorithm_finished, bool is_mission_complete, bool is_battery_exhausted);

    /**
     * @brief Moves the simulator to the next position.
     */
    void move(Step next_step);

public:
    Simulator() = default;
    /**
     * @brief Deleted copy constructor and assignment operator.
     *
     * The copy constructor and assignment operator are deleted to prevent using the 'shallow' raw pointer address copy (regarding `algorithm` data member).
     */
    Simulator(const Simulator& simulator) = delete;
    Simulator& operator=(const Simulator& simulator) = delete;

    void setAlgorithm(AbstractAlgorithm& algorithm);

    void readHouseFile(const std::string& house_file_path);

    /**
     * @brief Runs the cleaning operation.
     *
     * This method starts the cleaning operation and continues until one of the following conditions satisfied:
     * 1. cleaning mission is complete.
     * 2. the maximum number of steps is reached.
     * 3. vacuum cleaner mapped and cleaned all accessible positions.
     */
    void run();
};

#endif /* ROBOT_SIMULATOR_H_ */
