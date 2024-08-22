#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <deque>
#include <utility>
#include <functional>
#include <optional>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "common/abstract_algorithm.h"
#include "common/battery_meter.h"
#include "common/dirt_sensor.h"
#include "common/wall_sensor.h"
#include "common/position.h"
#include "common/enums.h"

#include "algorithm/path_tree.h"

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
    inline static const Position kDockingStationPosition = {0,0};       // Docking Station (relative) position.
    inline static const Direction kDirections[] = {                     // Directions of movement.
        Direction::North, Direction::East, Direction::South, Direction::West
    };

    struct HouseModel
    {
        std::unordered_map<Position, bool> wall_map;    // Internal algorithm's mapping of the house walls.
        std::unordered_set<Position> todo_positions;    // A set of positions to visit (unvisited / dirty positions).
    };

    struct BatteryModel
    {
        std::size_t full_capacity;                      // Battery full capacity.
        std::size_t amount_left;                        // Battery amount left.
    };

    struct CurrentTile
    {
        Position position = kDockingStationPosition;    // Robot current (relative) position.
        unsigned int dirt_level;                        // Robot current dirt level.
    };

    HouseModel house;
    BatteryModel battery;
    CurrentTile current_tile;

    std::optional<std::size_t> max_steps;               // Maximal allowed steps to take.
    std::size_t total_steps_left;                       // Number of allowed steps left.

    std::optional<const BatteryMeter*> battery_meter;   // Pointer to the battery meter.
    std::optional<const DirtSensor*> dirt_sensor;       // Pointer to the dirt sensor.
    std::optional<const WallsSensor*> walls_sensor;     // Pointer to the walls sensor.

    /**
     * @brief Checks if the algorithm is fully initialized.
     *
     * This method checks if the algorithm is fully initialized by checking if all the required components are initialized.
     * 
     * @throws std::runtime_error If algorithm is not fully initialized.
    */
    void assertAllInitialied() const 
    { 
        if (!(max_steps.has_value()
            && battery_meter.has_value() 
            && dirt_sensor.has_value()
            && walls_sensor.has_value()))
        {
                throw std::runtime_error("Algorithm is not fully initialized.");
        }
    }

    /**
     * @brief Performs a breadth-first search (BFS) to find a path that satisfies the given found_criteria.
     *
     * This method performs a BFS starting from the specified start_index in the path_tree.
     * It stops the search when the found_criteria function returns true for a position.
     * Note: All indices in the algorithm are indices of the path_tree data structure.
     *
     * @param path_tree The path tree to search in.
     * @param start_index The index to start the BFS from.
     * @param found_criteria The criteria function to determine if a position is found.
     * @return The index of the found position in the path_tree.
     */
    std::optional<std::size_t> performBFS(PathTree& path_tree,
                                          std::size_t start_index,
                                          std::function<bool(const Position&)> const & found_criteria) const;

    /**
     * @brief Calculates the distance of a path.
     *
     * This method calculates the distance of a path by counting the number of directions in the path.
     *
     * @param path The path to calculate the distance for.
     * @return The distance of the path.
     */
    std::size_t getPathDistance(const std::deque<Direction>& path) const { return path.size(); }

    /**
     * @brief Gets the next step in a path.
     *
     * This method gets the next step in a path by returning the front direction of the path.
     * If the path is empty, it returns Step::Stay.
     *
     * @param path The path to get the next step from.
     * @return The next step in the path.
     */
    Step getPathNextStep(const std::deque<Direction>& path) const
    {
        // Handle empty path
        if (path.empty())
        {
            return Step::Stay;
        }

        return static_cast<Step>(path.front());
    }

    /**
     * @brief Finds a path that satisfies the given criteria, relatively to a given start_position.
     *
     * This method finds a path that satisfies the given criteria by performing a BFS.
     *
     * @param start_position The position to start the path search from.
     * @param path The path to store the result in.
     * @param found_criteria The criteria function to determine if a position is found.
     * @return True if a path is found, false otherwise.
    */
    bool getPathByFoundCriteria(const Position& start_position,
                                std::deque<Direction>& path,
                                std::function<bool(const Position&)> const & found_criteria);

    /**
     * @brief Finds a path to the nearest position in the todo_positions set, relatively to a given start_position.
     *
     * This method finds a path to the nearest position in the todo_positions set by performing a BFS.
     *
     * @param start_position The position to start the path search from.
     * @param path The path to store the result in.
     * @return True if a path is found, false otherwise.
    */
    bool getPathToNearestTodo(const Position& start_position,
                              std::deque<Direction>& path);

    /**
     * @brief Finds a path to the station position.
     *
     * This method finds a path to the station position by performing a BFS.
     *
     * @param path The path to store the result in.
     * @return True if a path is found, false otherwise.
     */
    bool getPathToStation(std::deque<Direction>& path);

    /**
     * @brief Samples the wall sensor to detect the presence of a wall in positions adjacent to the current position.
     */
    void sampleWallSensor();

    /**
     * @brief Samples the dirt sensor to detect the dirt level at the current position.
     */
    void sampleDirtSensor();

    /**
     * @brief Samples the battery meter to get the amount of battery left.
     */
    void sampleBatteryMeter() { battery.amount_left = battery_meter.value()->getBatteryState(); }

    /**
     * @brief Gets the information from the sensors.
     *
     * This method gets the information from the sensors and updates the relevant data members.
     */
    void sampleSensors()
    {
        sampleWallSensor();
        sampleDirtSensor();
        sampleBatteryMeter();
    }

    /**
     * @brief Checks if the current position is the docking station position.
     * 
     * @return True if currently at Docking Station, false otherwise.
     */
    bool isAtDockingStation() const { return kDockingStationPosition == current_tile.position; }

    /**
     * @brief Checks if the battery is full.
     * 
     * @return True if battry is full, false otherwise.
     */
    bool isBatteryFull() const { return battery.amount_left == battery.full_capacity; }

    /**
     * @brief Gets the maximum reachable distance.
     * 
     * This method calculates the maximum reachable distance by taking the minimum of the max battery capacity and the total steps left.
     * 
     * @return Calculated maximal reachable distance.
     */
    std::size_t getMaxReachableDistance() const;

    /**
     * @brief Checks if all reachable positions are cleaned.
     * 
     * @return True if all reachable positions were cleaned, false otherwise.
     */
    bool isCleanedAllReachable();

    /**
     * @brief Checks if the algorithm should finish.
     *
     * This method checks if the algorithm should finish based on the current state.
     *
     * @param is_cleaned_all_reachable True if all reachable positions are cleaned, false otherwise.
     * @return True if the algorithm should finish, false otherwise.
     */
    bool shouldFinish(bool is_cleaned_all_reachable) const
    {
        bool is_finished_cleaning = isAtDockingStation() && is_cleaned_all_reachable;
        return (0 == total_steps_left) || is_finished_cleaning;
    }

    /**
     * @brief Checks if there are enough steps left to clean any position.
     * 
     * @return True if there are enough steps left to clean, false otherwise.
     */
    bool enoughStepsLeftToClean();

    /**
     * @brief Checks if the algorithm should keep charging.
     * 
     * @return True if algorithm should keep charging, false otherwise.
     */
    bool shouldKeepCharging() { return isAtDockingStation() && !isBatteryFull() && enoughStepsLeftToClean(); }

    /**
     * @brief Checks if the algorithm must return to station, and cannot stay or get further.
     * 
     * @param station_distance The distance to the docking station.
     * @return True if the algorithm should return to the docking station, false otherwise.
     */
    bool isTooLowBatteryToStay(std::size_t station_distance) const
    {
        std::size_t possible_steps_left = std::min(battery.amount_left, total_steps_left);
        return (possible_steps_left < 1 + station_distance);
    }

    /**
     * @brief Checks if the current position is dirty.
     * 
     * @return True if current position is dirty, false otherwise.
     */
    bool isCurrentPositionDirty() const { return current_tile.dirt_level > 0; }

    /**
     * @brief Checks if the battery is too low to get one more step away from the station.
     * 
     * @param station_distance The distance to the docking station.
     * @return True if the battery is too low to get further, false otherwise.
     */
    bool isTooLowBatteryToGetFurther(std::size_t station_distance) const
    {
        std::size_t possible_steps_left = std::min(battery.amount_left, total_steps_left);
        return possible_steps_left < 2 + station_distance;
    }

    /**
     * @brief Decides the next step based on the sensor information.
     *
     * This method decides the next step based on the dirt level, battery level, and battery full status.
     * 
     * @throws std::runtime_error If algorithm couldn't find a path back to the station.
     * @return The next step which should be taken.
     */
    Step decideNextStep();

    /**
     * @brief Decreases the algorithm's internal `total_steps_left` counter, while avoiding out of range values.
     * 
     * @throws std::runtime_error If `total_steps_left` becomes less than 0.
     */
    void safeDecreaseStepsLeft()
    {
        if (static_cast<int>(total_steps_left) - 1 < 0)
        {
            throw std::runtime_error("Robot exceeded the allowed maximal steps!");
        }

        total_steps_left = total_steps_left - 1;
    }

    /**
     * @brief Moves the algorithm's vacuum cleaner representation one step in the specified direction.
     *
     * This method moves the vacuum cleaner in the specified direction.
     *
     * @param step The direction to move in (or stay/finish).
     */
    void move(Step);

public:
    Algorithm() = default;
    /**
     * @brief Deleted copy constructor and assignment operator.
     *
     * The copy constructor and assignment operator are deleted to prevent using the 'shallow' raw pointers address copying made by `std::optional`.
     */
    Algorithm(const Algorithm& algorithm) = delete;
    Algorithm& operator=(const Algorithm& algorithm) = delete;

    /**
     * @brief Set the maximum number of steps the algorithm can take.
     *
     * @param maxSteps The maximum number of steps.
     */
    void setMaxSteps(std::size_t) override;

    /**
     * @brief Set the walls sensor for the algorithm.
     *
     * @param wallsSensor The walls sensor to use.
     */
    void setWallsSensor(const WallsSensor&) override;

    /**
     * @brief Set the dirt sensor for the algorithm.
     *
     * @param dirtSensor The dirt sensor to use.
     */
    void setDirtSensor(const DirtSensor&) override;

    /**
     * @brief Set the battery meter for the algorithm.
     *
     * @param batteryMeter The battery meter to use.
     */
    void setBatteryMeter(const BatteryMeter&) override;

    /**
     * @brief Get the next step to take.
     *
     * @return The next step to take.
     */
    Step nextStep() override;
};

#endif /* ALGORITHM_H_ */
