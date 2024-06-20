#ifndef VACUUM_NAVIGATIONSYSTEM_H_
#define VACUUM_NAVIGATIONSYSTEM_H_

#include "batterysensor.h"
#include "direction.h"
#include "dirtsensor.h"
#include "wallsensor.h"

#include <set>
#include <utility>
#include <vector>

class NavigationSystem {
        std::pair<int, int> current_position;
        std::set<std::pair<int, int>> wall_map;
        std::vector<std::pair<int, int>> todo_positions;
        BatterySensor& battery_sensor;
        DirtSensor& dirt_sensor;
        WallSensor& wall_sensor;

    public:
        NavigationSystem(BatterySensor&, DirtSensor&, WallSensor&);
        virtual Direction suggestNextStep();
        virtual void move(Direction);
        virtual std::vector<Direction>& getPathToStation();
};

#endif /* VACUUM_NAVIGATIONSYSTEM_H_ */