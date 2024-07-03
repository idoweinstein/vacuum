#ifndef VACUUM_NAVIGATIONSYSTEM_H_
#define VACUUM_NAVIGATIONSYSTEM_H_

#include <deque>
#include <utility>
#include <functional>
#include <unordered_set>
#include <unordered_map>

#include "batterysensor.h"
#include "dirtsensor.h"
#include "wallsensor.h"
#include "direction.h"
#include "position.h"
#include "pathtree.h"

class NavigationSystem {
        static constexpr const int kNotFound = -1;

        // wall_map := Internal algorithm's mapping of the house walls.
        std::unordered_map<Position, bool> wall_map;
        // toodo_positions := Set of positions to visit / to clean. It's empty when we've visited & cleaned all accessible positions.
        std::unordered_set<Position> todo_positions;

        Position current_position;
        BatterySensor& battery_sensor;
        DirtSensor& dirt_sensor;
        WallSensor& wall_sensor;

        const unsigned int full_battery;

        virtual int performBFS(PathTree& path_tree,
                               unsigned int start_index,
                               const std::function<bool(Position)>& found_criteria);

        virtual unsigned int getPathDistance(std::deque<Direction>& path) { return path.size(); }

        virtual Direction getPathNextStep(std::deque<Direction>& path)
        {
            // Handle empty path
            if (path.empty())
            {
                return Direction::STAY;
            }
            return path.front();
        }

        virtual bool getPathByFoundCriteria(std::deque<Direction>& path, const std::function<bool(Position)>& found_criteria);
        virtual bool getPathToNearestTodo(std::deque<Direction>& path);
        virtual bool getPathToStation(std::deque<Direction>& path);
        virtual void mapWallsAround();
        virtual void getSensorsInfo(unsigned int& dirt_level, float& battery_steps, bool& battery_is_full);
        virtual Direction decideNextStep(unsigned int dirt_level, float battery_steps, bool battery_is_full);

    public:
        NavigationSystem(BatterySensor&, DirtSensor&, WallSensor&);
        virtual Direction suggestNextStep();
        virtual void move(Direction);
};

#endif /* VACUUM_NAVIGATIONSYSTEM_H_ */
