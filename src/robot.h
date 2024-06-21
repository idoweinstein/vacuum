#ifndef VACUUM_ROBOT_H_
#define VACUUM_ROBOT_H_

#include <utility>
#include <vector>

#include "batterycontroller.h"
#include "navigationsystem.h"
#include "locationmanager.h"

class Robot
{
    unsigned int max_robot_steps;
    BatteryController battery_controller;
    NavigationSystem navigation_system;
    LocationManager location_manager;

public:
    Robot(int max_robot_steps,
          int max_battery_steps,
          std::vector<std::vector<bool>>& wall_map,
          std::vector<std::vector<unsigned int>>& dirt_map,
          std::pair<unsigned int, unsigned int>& docking_station_position);
}

#endif /* VACUUM_ROBOT_H_ */