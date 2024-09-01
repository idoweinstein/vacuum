#ifndef ROBOT_SIMULATOR_H_
#define ROBOT_SIMULATOR_H_

#include <vector>
#include <string>
#include <memory>
#include <sstream>

#include "common/abstract_algorithm.h"
#include "common/enums.h"
#include "common/position.h"

#include "deserializer.h"
#include "battery.h"
#include "status.h"
#include "house.h"

/**
 * @brief The SimulationStatistics struct represents a simulation results statistics report.
 * 
 * Some data members of this struct are computed on demand only!
 */
struct SimulationStatistics
{
    std::size_t num_steps_taken = 0;            // The total number of steps taken by the robot.
    std::vector<Step> step_history;             // The steps taken by the robot.
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
        NoAlgorithm,
        Ready
    };

    SimulationStatistics statistics;                    // Simulation Statistics (some fields are computed on demand)
    SimulatorState state = SimulatorState::NoAlgorithm; // Simulator's initialization current state.
    std::size_t max_simulator_steps;                    // Maximum number of steps the simulator can perform.
    House house;                                        // Simulator's house representation.
    Battery battery;                                    // Simulator's battery (for charging / discharging and getting battery level).
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
     * @brief Determines whether or not DEAD scoring condition applies.
     *
     * @param next_step The next step to be taken.
     */
    bool isDeadScoring(Step last_step)
    {
        return (Step::Finish != last_step && battery.isBatteryExhausted() && !house.isAtDockingStation());
    }

    /**
     * @brief Determines whether or not LYING scoring condition applies.
     *
     * @param next_step The next step to be taken.
     */
    bool isLyingScoring(Step last_step)
    {
        return (Step::Finish == last_step && !house.isAtDockingStation());
    }

    /**
     * @brief Calculates the score of the cleaning mission (stores result into SimulationStatistics).
     * 
     * @param next_step The next step to be taken.
     * 
     * A link to a further explanation about the scoring method:
     * https://moodle.tau.ac.il/mod/forum/discuss.php?d=109220
    */
    void calculateScore(Step last_step);

public:
    Simulator(const HouseFile& house_file);

    /**
     * @brief Deleted copy constructor and assignment operator.
     *
     * The copy constructor and assignment operator are deleted to prevent using the 'shallow' raw pointer address copy (regarding `algorithm` data member).
     */
    Simulator(const Simulator& simulator) = delete;
    Simulator& operator=(const Simulator& simulator) = delete;

    /**
     * @brief Returns the simulation max steps.
     */
    std::size_t getMaxSteps() const { return max_simulator_steps; }

    /**
     * @brief Computes the simulation Timeout score (in case needed - on timeout).
     */
    std::size_t getTimeoutScore() const { return (2 * max_simulator_steps + house.getInitialDirtCount() * kDirtFactor + kTimeoutPenalty); }

    /**
     * @brief Returns simulation statistics report.
     */
    const SimulationStatistics& getSimulationStatistics()
    {
        statistics.dirt_left = house.getTotalDirtCount();
        statistics.is_at_docking_station = house.isAtDockingStation();
        return statistics;
    }

    /**
     * @brief Sets the algorithm to be used by the simulator.
     *
     * @param algorithm The algorithm to be used.
     * 
     * @throws std::logic_error If this function was not called at the beginning of the initialization.
     */
    void setAlgorithm(AbstractAlgorithm& algorithm);

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
