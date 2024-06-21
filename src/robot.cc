#include "robot.h"

Robot::Robot(int max_robot_steps,
             int max_battery_steps,
             vector<vector<bool>>& wall_map,
             vector<vector<unsigned int>>& dirt_map,
             pair<unsigned int, unsigned int>& docking_station_position) : max_robot_steps(max_robot_steps)
{
    this.battery_controller = BatteryController(max_battery_steps);
    this.location_manager = LocationManager(wall_map, dirt_map, docking_station_position);
    this.navigation_algorithm = NavigationAlgorithm((BatterySensor)this.battery_controller,
                                                    (DirtSensor)this.location_manager,
                                                    (WallSensor)this.location_manager);
}