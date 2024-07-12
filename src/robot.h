#ifndef VACUUM_ROBOT_H_
#define VACUUM_ROBOT_H_

#include <vector>

#include "batterycontroller.h"
#include "navigationsystem.h"
#include "locationmanager.h"
#include "position.h"
#include "step.h"

/**
 * @brief The Robot class represents a vacuum cleaning robot.
 *
 * This class encapsulates the main functionality of a vacuum cleaning robot.
 */
class Robot
{
    unsigned int max_robot_steps = 0;     // The maximum number of steps the robot can perform.
    BatteryController battery_controller; // The battery controller for managing the robot's battery.
    LocationManager location_manager;     // The location manager for tracking the robot's position.
    NavigationSystem navigation_system;   // The navigation system for guiding the robot's movement.

    /**
     * @brief Checks if the cleaning mission is complete.
     *
     * @return true if the cleaning mission is complete, false otherwise.
     */
    bool isMissionComplete() const { return (location_manager.isFinished() && location_manager.isInDockingStation()); }

    /**
     * @brief Checks if the robot should stop cleaning.
     *
     * The robot should stop cleaning if the cleaning mission is complete or
     * if the maximum number of steps has been performed.
     *
     * @param steps_performed The number of steps performed by the robot.
     * @return true if the robot should stop cleaning, false otherwise.
     */
    bool shouldStopCleaning(unsigned int steps_performed) const
    {
        bool is_max_steps_performed = (steps_performed >= max_robot_steps);
        return (isMissionComplete() || is_max_steps_performed);
    }

     /**
     * @brief Moves the robot to the next position.
     */
    void move(Step next_step);

public:
    /**
     * @brief Constructs a new Robot object.
     *
     * @param max_robot_steps The maximum number of steps the robot can perform.
     * @param max_battery_steps The maximum number of steps the battery can last.
     * @param wall_map The map representing the walls in the environment.
     * @param dirt_map The map representing the dirt in the environment.
     * @param docking_station_position The position of the docking station.
     */
    explicit Robot(unsigned int max_robot_steps,
                   unsigned int max_battery_steps,
                   std::vector<std::vector<bool>>& wall_map,
                   std::vector<std::vector<unsigned int>>& dirt_map,
                   Position& docking_station_position);

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

#endif /* VACUUM_ROBOT_H_ */
