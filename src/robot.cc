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