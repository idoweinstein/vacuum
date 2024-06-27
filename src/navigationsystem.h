#ifndef VACUUM_NAVIGATIONSYSTEM_H_
#define VACUUM_NAVIGATIONSYSTEM_H_

#include "batterysensor.h"
#include "direction.h"
#include "dirtsensor.h"
#include "position.h"
#include "wallsensor.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class NavigationSystem {
        Position current_position;
        std::unordered_map<Position, bool> wall_map;
        std::unordered_set<Position> todo_positions;
        BatterySensor& battery_sensor;
        DirtSensor& dirt_sensor;
        WallSensor& wall_sensor;

        virtual std::vector<Direction>* performBFS(Position, std::function<bool(Position)>);
        virtual std::vector<Direction>* getPathToNearestTodo();

        /* Subfunctions of suggestNextStep */
        virtual void mapWallsAround();
        virtual void getSensorsInfo(unsigned int& dirt_level, float& battery_steps);
        virtual Direction decideNextStep(unsigned int dirt_level, float battery_steps);

    public:
        NavigationSystem(BatterySensor&, DirtSensor&, WallSensor&);
        virtual Direction suggestNextStep();
        virtual void move(Direction);
        virtual std::vector<Direction>& getPathToStation();
};

#endif /* VACUUM_NAVIGATIONSYSTEM_H_ */