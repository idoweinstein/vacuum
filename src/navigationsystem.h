#ifndef VACUUM_NAVIGATIONSYSTEM_H_
#define VACUUM_NAVIGATIONSYSTEM_H_

#include "batterysensor.h"
#include "dirtsensor.h"
#include "wallsensor.h"
#include "direction.h"
#include "position.h"
#include "pathtree.h"

#include <deque>
#include <utility>
#include <functional>
#include <unordered_set>
#include <unordered_map>

class NavigationSystem {
        Position current_position;
        std::unordered_map<Position, bool> wall_map;
        std::unordered_set<Position> todo_positions;
        BatterySensor& battery_sensor;
        DirtSensor& dirt_sensor;
        WallSensor& wall_sensor;

        const int kNotFound = -1;

        virtual int performBFS(PathTree& path_tree, 
                               unsigned int start_index,
                               std::function<bool(Position)> found_criteria);
        virtual bool getPathByFoundCriteria(std::deque<Direction>& path, std::function<bool(Position)> found_criteria);
        virtual bool getPathToNearestTodo(std::deque<Direction>& path);
        virtual bool getPathToStation(std::deque<Direction>& path);
        virtual void mapWallsAround();
        virtual void getSensorsInfo(unsigned int& dirt_level, float& battery_steps);
        virtual Direction decideNextStep(unsigned int dirt_level, float battery_steps);

    public:
        NavigationSystem(BatterySensor&, DirtSensor&, WallSensor&);
        virtual Direction suggestNextStep();
        virtual void move(Direction);
};

#endif /* VACUUM_NAVIGATIONSYSTEM_H_ */