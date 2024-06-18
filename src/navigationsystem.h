#ifndef VACUUM_NAVIGATIONSYSTEM_H_
#define VACUUM_NAVIGATIONSYSTEM_H_

#include "batterysensor.h"
#include "direction.h"
#include "dirtsensor.h"
#include "wallsensor.h"

#include <set>
#include <tuple>
#include <vector>

using namespace std;

class NavigationSystem {
        tuple<int, int> current_position;
        set<tuple<int, int>> wall_map;
        vector<tuple<int, int>> todo_positions;
        BatterySensor& battery_sensor;
        DirtSensor& dirt_sensor;
        WallSensor& wall_sensor;

    public:
        NavigationSystem(BatterySensor&, DirtSensor&, WallSensor&);
        virtual Direction suggestNextStep();
        virtual void move(Direction);
        virtual vector<Direction>& getPathToStation();
};

#endif /* VACUUM_NAVIGATIONSYSTEM_H_ */