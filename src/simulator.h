#ifndef VACUUM_SIMULATOR_H_
#define VACUUM_SIMULATOR_H_

#include <vector>
#include <string>
#include <memory>

#include "abstractalgorithm.h"
#include "position.h"
#include "battery.h"
#include "status.h"
#include "house.h"
#include "step.h"

/**
 * @brief The Simulator class represents a vacuum cleaning robot.
 *
 * This class encapsulates the main functionality of a vacuum cleaning robot.
 */
class Simulator
{
    class NullAlgorithm : public AbstractAlgorithm
    {
    private:
        // Make the constructor private to prevent instantiation.
        NullAlgorithm() = default;
    public:
        void setMaxSteps(std::size_t) override {}
        void setWallsSensor(const WallsSensor&) override {}
        void setDirtSensor(const DirtSensor&) override {}
        void setBatteryMeter(const BatteryMeter&) override {}
        Step nextStep() override { return Step::FINISH; }
        // Implement singleton pattern for NullAlgorithm
        static NullAlgorithm& getInstance() {
            static NullAlgorithm instance;
            return instance;
        }
    };

    unsigned int max_simulator_steps = 0; // The maximum number of steps the simulator can perform.
    std::unique_ptr<Battery> battery; // The battery controller for managing the robot's battery.
    std::unique_ptr<House> house;     // The location manager for tracking the robot's position.
    // The navigation system for guiding the robot's movement.
    std::shared_ptr<AbstractAlgorithm> algorithm = std::shared_ptr<NullAlgorithm>(&NullAlgorithm::getInstance());
    /**
     * @brief Checks if the cleaning mission is complete.
     *
     * @return true if the cleaning mission is complete, false otherwise.
     */
    bool isMissionComplete() const { return (house->isFinished() && house->isInDockingStation()); }

    /**
     * @brief Checks if the simulator should stop cleaning.
     *
     * The simulator should stop cleaning if the cleaning mission is complete or
     * if the maximum number of steps has been performed.
     *
     * @param steps_performed The number of steps performed by the simulator.
     * @return true if the simulator should stop cleaning, false otherwise.
     */
    bool shouldStopCleaning(unsigned int steps_performed) const
    {
        bool is_max_steps_performed = (steps_performed >= max_simulator_steps);
        return (isMissionComplete() || is_max_steps_performed);
    }

    Status getMissionStatus(bool is_algorithm_finished, bool is_mission_complete, bool is_battery_exhausted);

     /**
     * @brief Moves the simulator to the next position.
     */
    void move(Step next_step);

public:
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

#endif /* VACUUM_SIMULATOR_H_ */
