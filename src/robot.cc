#include "robot.h"

#include "robotlogger.h"

Robot::Robot(unsigned int max_robot_steps,
             unsigned int max_battery_steps,
             std::vector<std::vector<bool>>& wall_map,
             std::vector<std::vector<unsigned int>>& dirt_map,
             Position& docking_station_position)
    : max_robot_steps(max_robot_steps),
      battery_controller(max_battery_steps),
      location_manager(wall_map, dirt_map, docking_station_position),
      navigation_system(static_cast<BatteryMeter&>(battery_controller),
                        static_cast<DirtSensor&>(location_manager),
                        static_cast<WallsSensor&>(location_manager))
{ }

void Robot::move(Step next_step)
{
    RobotLogger& logger = RobotLogger::getInstance();

    if (Step::FINISH == next_step)
    {
        /* Shouldn't happen */
        return;
    }

    /* If no battery left - discharge() throws an Empty Battery exception */
    if (Step::STAY == next_step)
    {
        if (location_manager.isInDockingStation())
        {
            battery_controller.charge();
        }

        else
        {
            battery_controller.discharge();
            location_manager.cleanCurrentPosition();
        }
    }

    else
    {
        battery_controller.discharge();
    }
 
    navigation_system.move(next_step);
    location_manager.move(next_step);

    Position next_position = location_manager.getCurrentPosition();
    logger.logRobotStep(next_step, next_position);
}

void Robot::run()
{
    RobotLogger& logger = RobotLogger::getInstance();
    unsigned int total_steps_performed = 0;
    bool is_algorithm_finished = false;

    while (!shouldStopCleaning(total_steps_performed))
    {
        Step next_step = navigation_system.suggestNextStep();

        if (Step::FINISH == next_step)
        {
            // In case Robot mapped all accessible positions and have nothing left to clean
            is_algorithm_finished = true;
            break;
        }

        move(next_step);
        total_steps_performed++;
    }

    if (isMissionComplete() || is_algorithm_finished)
    {
        logger.logRobotFinish();
    }

    unsigned int total_dirt_count = location_manager.getTotalDirtCount();
    bool is_battery_exhausted = battery_controller.getBatteryState() < 1;
    bool is_mission_complete = (0 == total_dirt_count) && location_manager.isInDockingStation();

    logger.logCleaningStatistics(total_steps_performed,
                                 total_dirt_count,
                                 is_battery_exhausted,
                                 is_mission_complete);
}
