#ifndef ROBOT_SIMULATOR_H_
#define ROBOT_SIMULATOR_H_

#include <vector>
#include <string>
#include <memory>

#include "common/abstract_algorithm.h"
#include "common/enums.h"
#include "common/position.h"

#include "battery.h"
#include "status.h"
#include "house.h"

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

    std::size_t total_steps_taken = 0;
    SimulatorState state = SimulatorState::Initial;     // Simulator's initialization current state.
    Status mission_status = Status::Working;            // Simulator's mission status.
    unsigned int max_simulator_steps = 0;               // Maximum number of steps the simulator can perform.
    std::unique_ptr<Battery> battery = nullptr;         // Simulator's battery (for charging / discharging and getting battery level).
    std::unique_ptr<House> house = nullptr;             // Simulator's house representation.
    AbstractAlgorithm* algorithm = nullptr;             // Simulator's algorithm to suggest its next steps.

    /* Scoring */
    static const std::size_t kDeadPenalty = 2000;        // The penalty for a dead robot.
    static const std::size_t kLying = 3000;              // The penalty for a lying algorithm.
    static const std::size_t kNotStation = 1000;         // The penalty for finishing not in station.
    static const std::size_t kDirtFactor = 300;          // The factor for each dirt level in the score.

    /**
     * @brief Updates the status of the mission.
     *
     * @param next_step The next step to be taken.
     */
    void updateMissionStatus(Step next_step);

    /**
     * @brief Moves the simulator to the next position.
     */
    void move(Step next_step);

    /**
     * @brief Calculates the score of the cleaning mission.
     * 
     * @param last_step The last step taken by the robot.
     * @param dirt_count The total dirt count in the house.
     * @param steps_taken The total steps taken by the robot.
     * @param is_at_docking_station Whether the robot is at the docking station.
     * @return The score of the cleaning mission.
     */
    std::size_t calculateScore(Step last_step, std::size_t dirt_count, std::size_t steps_taken, bool is_at_docking_station) const;

public:
    Simulator() = default;
    /**
     * @brief Deleted copy constructor and assignment operator.
     *
     * The copy constructor and assignment operator are deleted to prevent using the 'shallow' raw pointer address copy (regarding `algorithm` data member).
     */
    Simulator(const Simulator& simulator) = delete;
    Simulator& operator=(const Simulator& simulator) = delete;

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
     * @throws std::logic_error If the simulator is not properly initialized yet.
     */
    void run();
};

#endif /* ROBOT_SIMULATOR_H_ */
