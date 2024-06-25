#ifndef VACUUM_ROBOT_H_
#define VACUUM_ROBOT_H_

#include <utility>
#include <vector>

#include "batterycontroller.h"
#include "navigationsystem.h"
#include "locationmanager.h"
#include "position.h"

class Robot
{
    unsigned int max_robot_steps = 0;
    BatteryController battery_controller;
    LocationManager location_manager;
    NavigationSystem navigation_system;

public:
    Robot(unsigned int max_robot_steps,
          unsigned int max_battery_steps,
          std::vector<std::vector<bool>>& wall_map,
          std::vector<std::vector<unsigned int>>& dirt_map,
          UPosition& docking_station_position);
};

#endif /* VACUUM_ROBOT_H_ */