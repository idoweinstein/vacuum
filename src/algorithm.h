#ifndef VACUUM_ALGORITHM_H_
#define VACUUM_ALGORITHM_H_

#include <deque>
#include <utility>
#include <functional>
#include <optional>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "abstractalgorithm.h"
#include "batterymeter.h"
#include "dirtsensor.h"
#include "wallssensor.h"
#include "direction.h"
#include "position.h"
#include "pathtree.h"
#include "step.h"

/**
 * @class Algorithm 
 * @brief The Algorithm class represents an implementation of the navigation algorithm of a vacuum cleaner.
 *
 * The Algorithm class is responsible for managing the movement and navigation of the vacuum cleaner.
 * It uses various sensors such as BatteryMeter, DirtSensor, and WallsSensor to make decisions about the next step.
 * The navigation system keeps track of the current position, wall map, and a set of positions to visit.
 * It uses a path tree to store the paths explored during navigation.
 *
 * The class provides methods for suggesting the next step and moving the vacuum cleaner in a specific direction.
 */
class Algorithm : public AbstractAlgorithm
{
        static constexpr const int kNotFound = -1;     // Constant value representing path not found status.
        inline static const Direction directions[] = { // Directions of movement.
            Direction::North, Direction::East, Direction::South, Direction::West
        };

        std::size_t steps_taken = 0;                    // Number of steps taken so far.

        std::optional<std::size_t> max_steps;           // Maximum number of steps to take.

        std::unordered_map<Position, bool> wall_map;    // Internal algorithm's mapping of the house walls.
        std::unordered_set<Position> todo_positions;    // A set of positions to visit (unvisited / dirty positions).

        Position current_position;                      // The current position of the vacuum cleaner.

        std::optional<const BatteryMeter*>
            battery_meter;                              // Reference to the battery meter.
        unsigned int full_battery;                      // Full battery power (in steps).

        std::optional<const DirtSensor*> dirt_sensor;   // Reference to the dirt sensor.

        std::optional<const WallsSensor*> walls_sensor; // Reference to the walls sensor.

        /**
         * @brief Checks if the algorithm is fully initialized.
         *
         * This method checks if the algorithm is fully initialized by checking if all the required components are initialized.
         */
        void assertAllInitialied() const 
        { 
            if (!(max_steps.has_value()
                  && battery_meter.has_value() 
                  && dirt_sensor.has_value()
                  && walls_sensor.has_value())) {
                throw std::runtime_error("Algorithm is not fully initialized.");
            }
        }

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
         * If the path is empty, it returns Step::Stay.
         *
         * @param path The path to get the next step from.
         * @return The next step in the path.
         */
        virtual Step getPathNextStep(const std::deque<Direction>& path) const
        {
            // Handle empty path
            if (path.empty())
            {
                return Step::Stay;
            }
            return static_cast<Step>(path.front());
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
         * @param remaining_steps_until_charge The variable to store the remaining steps until charge.
         * @param remaining_steps_total The variable to store the remaining total steps.
         * @param battery_is_full The variable to store the battery full status.
         */
        virtual void getSensorsInfo(int& dirt_level, std::size_t& remaining_steps_until_charge, std::size_t& remaining_steps_total, bool& battery_is_full);

        /**
         * @brief Decides the next step based on the sensor information.
         *
         * This method decides the next step based on the dirt level, battery level, and battery full status.
         *
         * @param dirt_level The dirt level.
         * @param remaining_steps_until_charge The remaining steps until charge.
         * @param remaining_steps_total The remaining total steps.
         * @param battery_is_full The battery full status.
         * @return The next step to take.
         */
        virtual Step decideNextStep(int dirt_level, std::size_t remaining_steps_until_charge, std::size_t remaining_steps_total, bool battery_is_full);

        /**
         * @brief Moves the algorithm's vacuum cleaner representation one step in the specified direction.
         *
         * This method moves the vacuum cleaner in the specified direction.
         *
         * @param step The direction to move in (or stay/finish).
         */
        virtual void move(Step);

    public:
        /**
         * @brief Constructs a new Algorithm object.
         */
        Algorithm();

        /**
         * @brief Set the maximum number of steps the algorithm can take.
         *
         * @param maxSteps The maximum number of steps.
         */
        virtual void setMaxSteps(std::size_t) override;

        /**
         * @brief Set the walls sensor for the algorithm.
         *
         * @param wallsSensor The walls sensor to use.
         */
        virtual void setWallsSensor(const WallsSensor&) override;

        /**
         * @brief Set the dirt sensor for the algorithm.
         *
         * @param dirtSensor The dirt sensor to use.
         */
        virtual void setDirtSensor(const DirtSensor&) override;

        /**
         * @brief Set the battery meter for the algorithm.
         *
         * @param batteryMeter The battery meter to use.
         */
        virtual void setBatteryMeter(const BatteryMeter&) override;

        /**
         * @brief Get the next step to take.
         *
         * @return The next step to take.
         */
        virtual Step nextStep() override;
};

#endif /* VACUUM_ALGORITHM_H_ */
