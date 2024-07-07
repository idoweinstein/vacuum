#ifndef VACUUM_NAVIGATIONSYSTEM_H_
#define VACUUM_NAVIGATIONSYSTEM_H_

#include <deque>
#include <utility>
#include <functional>
#include <unordered_set>
#include <unordered_map>

#include "batterymeter.h"
#include "dirtsensor.h"
#include "wallssensor.h"
#include "direction.h"
#include "position.h"
#include "pathtree.h"

/**
 * @class NavigationSystem
 * @brief The NavigationSystem class represents the navigation algorithm of a vacuum cleaner.
 *
 * The NavigationSystem class is responsible for managing the movement and navigation of the vacuum cleaner.
 * It uses various sensors such as BatteryMeter, DirtSensor, and WallsSensor to make decisions about the next step.
 * The navigation system keeps track of the current position, wall map, and a set of positions to visit.
 * It uses a path tree to store the paths explored during navigation.
 *
 * The class provides methods for suggesting the next step and moving the vacuum cleaner in a specific direction.
 */
class NavigationSystem
{
        static constexpr const int kNotFound = -1;     // Constant value representing path not found status.
        inline static const Direction directions[] = { // Directions of movement.
            Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST
        };

        std::unordered_map<Position, bool> wall_map;    // Internal algorithm's mapping of the house walls.
        std::unordered_set<Position> todo_positions;    // A set of positions to visit (unvisited / dirty positions).

        Position current_position;                      // The current position of the vacuum cleaner.
        BatteryMeter& battery_meter;                    // Reference to the battery meter.
        DirtSensor& dirt_sensor;                        // Reference to the dirt sensor.
        WallsSensor& wall_sensor;                        // Reference to the wall sensor.

        const unsigned int full_battery;                // Full battery power (in steps).

        /**
         * @brief Performs a breadth-first search (BFS) to find a path that satisfies the given criteria.
         *
         * This method performs a BFS starting from the specified start_index in the path_tree.
         * It stops the search when the found_criteria function returns true for a position.
         * Note: All indices in the algorithm are indices of the path_tree data structure.
         *
         * @param path_tree The path tree to search in.
         * @param start_index The index to start the BFS from.
         * @param found_criteria The criteria function to determine if a position is found.
         * @return The index of the found position in the path_tree, or kNotFound if not found.
         */
        virtual int performBFS(PathTree& path_tree,
                               unsigned int start_index,
                               const std::function<bool(Position)>& found_criteria) const;

        /**
         * @brief Calculates the distance of a path.
         *
         * This method calculates the distance of a path by counting the number of directions in the path.
         *
         * @param path The path to calculate the distance for.
         * @return The distance of the path.
         */
        virtual unsigned int getPathDistance(const std::deque<Direction>& path) const { return path.size(); }

        /**
         * @brief Gets the next step in a path.
         *
         * This method gets the next step in a path by returning the front direction of the path.
         * If the path is empty, it returns Direction::STAY.
         *
         * @param path The path to get the next step from.
         * @return The next step in the path.
         */
        virtual Direction getPathNextStep(const std::deque<Direction>& path) const

        {
            // Handle empty path
            if (path.empty())
            {
                return Direction::STAY;
            }
            return path.front();
        }

        /**
         * @brief Finds a path that satisfies the given criteria.
         *
         * This method finds a path that satisfies the given criteria by performing a BFS.
         *
         * @param path The path to store the result in.
         * @param found_criteria The criteria function to determine if a position is found.
         * @return True if a path is found, false otherwise.
         */
        virtual bool getPathByFoundCriteria(std::deque<Direction>& path, const std::function<bool(Position)>& found_criteria);

        /**
         * @brief Finds a path to the nearest position in the todo_positions set.
         *
         * This method finds a path to the nearest position in the todo_positions set by performing a BFS.
         *
         * @param path The path to store the result in.
         * @return True if a path is found, false otherwise.
         */
        virtual bool getPathToNearestTodo(std::deque<Direction>& path);

        /**
         * @brief Finds a path to the station position.
         *
         * This method finds a path to the station position by performing a BFS.
         *
         * @param path The path to store the result in.
         * @return True if a path is found, false otherwise.
         */
        virtual bool getPathToStation(std::deque<Direction>& path);

        /**
         * @brief Maps the walls around the current position.
         *
         * This method maps the walls around the current position by updating the wall_map.
         */
        virtual void mapWallsAround();

        /**
         * @brief Gets the information from the sensors.
         *
         * This method gets the information from the sensors and updates the provided variables.
         *
         * @param dirt_level The variable to store the dirt level.
         * @param battery_steps The variable to store the battery level in steps.
         * @param battery_is_full The variable to store the battery full status.
         */
        virtual void getSensorsInfo(unsigned int& dirt_level, std::size_t& battery_steps, bool& battery_is_full);

        /**
         * @brief Decides the next step based on the sensor information.
         *
         * This method decides the next step based on the dirt level, battery level, and battery full status.
         *
         * @param dirt_level The dirt level.
         * @param battery_steps The battery level in steps.
         * @param battery_is_full The battery full status.
         * @return The next step to take.
         */
        virtual Direction decideNextStep(unsigned int dirt_level, std::size_t battery_steps, bool battery_is_full);

    public:
        /**
         * @brief Constructs a new NavigationSystem object.
         *
         * @param battery_meter The battery meter (sensor).
         * @param dirt_sensor The dirt sensor.
         * @param wall_sensor The wall sensor.
         */
        explicit NavigationSystem(BatteryMeter&, DirtSensor&, WallsSensor&);

        /**
         * @brief Suggests the next step for the vacuum cleaner.
         *
         * This method suggests the next step for the vacuum cleaner based on the sensor information.
         *
         * @return The suggested next step.
         */
        virtual Direction suggestNextStep();

        /**
         * @brief Moves the algorithm's vacuum cleaner representation in the specified direction.
         *
         * This method moves the vacuum cleaner in the specified direction.
         *
         * @param direction The direction to move in.
         */
        virtual void move(Direction);
};

#endif /* VACUUM_NAVIGATIONSYSTEM_H_ */
