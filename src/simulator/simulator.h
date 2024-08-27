#ifndef ROBOT_SIMULATOR_H_
#define ROBOT_SIMULATOR_H_

#include <vector>
#include <string>
#include <memory>
#include <sstream>

#include "common/abstract_algorithm.h"
#include "common/enums.h"
#include "common/position.h"

#include "battery.h"
#include "status.h"
#include "house.h"

struct SimulationStatistics
{
    std::size_t total_steps_taken = 0;          // The total steps taken by the robot.
    std::vector<Step> steps_taken;              // The steps taken by the robot.
    std::size_t dirt_left;                      // The dirt amount left at simulation end (computed on demand).
    bool is_at_docking_station;                 // Whether or not the robot is at docking station at simulation end (computed on demand).
    Status mission_status = Status::Working;    // Simulation's final mission status (Finished / Working / Dead).
    std::size_t score;                          // Simulation's final score (computed on graceful finish only).
};

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

    SimulationStatistics statistics;                    // Simulation Statistics (some fields are computed on demand)
    SimulatorState state = SimulatorState::Initial;     // Simulator's initialization current state.
    std::size_t max_simulator_steps = 0;                // Maximum number of steps the simulator can perform.
    std::unique_ptr<Battery> battery = nullptr;         // Simulator's battery (for charging / discharging and getting battery level).
    std::unique_ptr<House> house = nullptr;             // Simulator's house representation.
    AbstractAlgorithm* algorithm = nullptr;             // Simulator's algorithm to suggest its next steps.

    /* Scoring */
    static const std::size_t kDeadPenalty = 2000;        // The penalty for a dead robot.
    static const std::size_t kTimeoutPenalty = 2000;     // The penalty for an algorithm timeout.
    static const std::size_t kLyingPenalty = 3000;       // The penalty for a lying algorithm.
    static const std::size_t kNotInDockPenalty = 1000;   // The penalty for finishing not in docking station.
    static const std::size_t kDirtFactor = 300;          // The factor for each dirt level in the score.

    /**
     * @brief Updates the status of the mission (stores result into SimulationStatistics).
     *
     * @param next_step The next step to be taken.
     */
    void updateMissionStatus(Step next_step);

    /**
     * @brief Moves the simulator to the next position.
     */
    void move(Step next_step);

    /**
     * @brief Calculates the score of the cleaning mission (stores result into SimulationStatistics).
    */
    void calculateScore();

public:
    Simulator() = default;
    /**
     * @brief Deleted copy constructor and assignment operator.
     *
     * The copy constructor and assignment operator are deleted to prevent using the 'shallow' raw pointer address copy (regarding `algorithm` data member).
     */
    Simulator(const Simulator& simulator) = delete;
    Simulator& operator=(const Simulator& simulator) = delete;

    std::size_t getMaxSteps() const { return max_simulator_steps; }

    std::size_t getTimeoutScore() const { return (2 * max_simulator_steps + house->getInitialDirtCount() * kDirtFactor + kTimeoutPenalty); }

    SimulationStatistics& getSimulationStatistics()
    {
        statistics.dirt_left = house->getTotalDirtCount();
        statistics.is_at_docking_station = house->isAtDockingStation();
        return statistics;
    }

    /**
     * @brief Sets the algorithm to be used by the simulator.
     *
     * @param algorithm The algorithm to be used.
     * @throws std::logic_error If this function was not called at the beginning of the initialization.
     */
    void setAlgorithm(AbstractAlgorithm& algorithm);

    /**
     * @brief Reads the house representation of a given input file.
     *
     * @param house_file_path The file path which the house will be read from.
     * @throws std::logic_error If the simulator is already fully initialized.
     * @throws std::runtime_error If couldn't open given file.
     */
    void readHouseFile(const std::string& house_file_path);

    /**
     * @brief Runs the cleaning operation.
     *
     * This method starts the cleaning operation and continues until one of the following conditions satisfied:
     * 1. cleaning mission is complete.
     * 2. the maximum number of steps is reached.
     * 3. vacuum cleaner mapped and cleaned all accessible positions.
     * 
     * @returns The simulation result of the cleaning mission.
     * 
     * @throws std::logic_error If the simulator is not properly initialized yet.
     */
    std::size_t run();
};

#endif /* ROBOT_SIMULATOR_H_ */
