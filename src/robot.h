#ifndef VACUUM_ROBOT_H_
#define VACUUM_ROBOT_H_

#include <utility>
#include <vector>

#include "batterycontroller.h"
#include "navigationsystem.h"
#include "locationmanager.h"
#include "direction.h"
#include "position.h"

class Robot
{
    unsigned int max_robot_steps = 0;
    BatteryController battery_controller;
    LocationManager location_manager;
    NavigationSystem navigation_system;

    bool isMissionComplete() { return (this->location_manager.isFinished() && this->location_manager.isInDockingStation()); }
    bool shouldStopCleaning(unsigned int steps_performed)
    {
        bool is_max_steps_performed = (steps_performed >= this->max_robot_steps);
        return (isMissionComplete() || is_max_steps_performed);
    }

    void move(void);

public:
    Robot(unsigned int max_robot_steps,
          unsigned int max_battery_steps,
          std::vector<std::vector<bool>>& wall_map,
          std::vector<std::vector<unsigned int>>& dirt_map,
          UPosition& docking_station_position);

    void run(void);
};

#endif /* VACUUM_ROBOT_H_ */