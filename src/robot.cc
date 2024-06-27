#include "robot.h"

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
    Direction next_direction = this->navigation_system.suggestNextStep();

    /* If no battery left - discharge() throws an Empty Battery exception */
    if (next_direction == Direction::STAY)
    {
        if (this->location_manager.isInDockingStation())
        {
            this->battery_controller.charge();            
        }
        else
        {
            this->battery_controller.discharge();
            this->location_manager.cleanCurrentPoisition();
        }
    }
    else
    {
        this->battery_controller.discharge();
    }
    
    this->navigation_system.move(next_direction);
    this->location_manager.move(next_direction);

}

void Robot::run(void)
{
    unsigned int steps_performed = 0;
    while (!this->shouldStopCleaning(steps_performed))
    {
        this->move();
        steps_performed++;
    }
}