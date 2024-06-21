#include "robot.h"

Robot::Robot(unsigned int max_robot_steps,
             unsigned int max_battery_steps,
             std::vector<std::vector<bool>>& wall_map,
             std::vector<std::vector<unsigned int>>& dirt_map,
             std::pair<unsigned int, unsigned int>& docking_station_position) : max_robot_steps(max_robot_steps),
                                                                                battery_controller(max_battery_steps),
                                                                                location_manager(wall_map, dirt_map, docking_station_position),
                                                                                navigation_system((BatterySensor&)battery_controller,
                                                                                                  (DirtSensor&)location_manager,
                                                                                                  (WallSensor&)location_manager)
{ }