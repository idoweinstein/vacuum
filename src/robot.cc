#include "robot.h"
#include "robotlogger.h"

Robot::Robot(unsigned int max_robot_steps,
             unsigned int max_battery_steps,
             std::vector<std::vector<bool>>& wall_map,
             std::vector<std::vector<unsigned int>>& dirt_map,
             UPosition& docking_station_position)
    : max_robot_steps(max_robot_steps),
      battery_controller(max_battery_steps),
      location_manager(wall_map, dirt_map, docking_station_position),
      navigation_system(
            static_cast<BatterySensor&>(battery_controller),
            static_cast<DirtSensor&>(location_manager),
            static_cast<WallSensor&>(location_manager))
{ }

void Robot::move(void)
{
    RobotLogger& logger = RobotLogger::getInstance();
    Direction next_direction = this->navigation_system.suggestNextStep();

    /* If no battery left - throws Empty Battery exception */
    if (next_direction == Direction::STAY)
    {
        if (this->location_manager.isInDockingStation())
        {
            this->battery_controller.charge();            
        }
        else
        {
            this->location_manager.cleanCurrentPoisition();
            this->battery_controller.discharge();
        }
    }
    else
    {
        this->battery_controller.discharge();
    }
    
    this->navigation_system.move(next_direction);
    this->location_manager.move(next_direction);

    Position next_position = this->location_manager.getCurrentPosition();
    logger.logRobotStep(next_direction, next_position);
}

void Robot::run(void)
{
    RobotLogger& logger = RobotLogger::getInstance();
    unsigned int total_steps_performed = 0;

    while (!this->shouldStopCleaning(total_steps_performed))
    {
        this->move();
        total_steps_performed++;
    }

    unsigned int total_dirt_count = this->location_manager.getTotalDirtCount();
    bool is_battery_exhausted = this->battery_controller.getCurrentAmount() < 1;
    bool is_mission_complete = total_dirt_count == 0 && this->location_manager.isInDockingStation();

    logger.logCleaningStatistics(total_steps_performed,
                                 total_dirt_count,
                                 is_battery_exhausted,
                                 is_mission_complete);
}