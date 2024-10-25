#include "base_algorithm.h"

#include "algorithm/AlgorithmRegistration.h"

#include <map>
#include <cmath>
#include <queue>
#include <deque>
#include <stdexcept>
#include <unordered_set>


#include <iostream>
#include <syncstream>
#include <thread>

#include <unistd.h>   // for getpid()
#include <sys/syscall.h> // for syscall() and SYS_gettid

static void print(const std::string& message)
{
    std::osyncstream(std::cout) << "(" << syscall(SYS_gettid) << ") " << message << std::endl;
}

std::optional<std::size_t> BaseAlgorithm::buildPathTree(PathTree& path_tree,
                                                        std::size_t max_depth,
                                                        std::size_t start_index,
                                                        std::function<bool(const Position&)> const & found_criteria) const
{
    std::queue<std::size_t> index_queue;
    std::optional<std::size_t> path_end_index;

    print("buildPathTree start");

    // If current position satisfies found_criteria - Return empty path
    if (found_criteria(path_tree.getPosition(start_index)))
    {
        path_end_index = start_index;
        return path_end_index;
    }

    // std::cout << "Entered buildPathTree with position not satisfying found_criteria" << std::endl;
    // std::cout << "Max depth allowed: " << max_depth << std::endl;

    // std::size_t current_depth = 0;

    index_queue.push(start_index);

    // Perform BFS like traversal
    while (!index_queue.empty())
    {
        std::size_t parent_index = index_queue.front();
        index_queue.pop();

        // if (path_tree.getDepth(parent_index) > current_depth)
        // {
        //     current_depth = path_tree.getDepth(parent_index);
        //     std::cout << "Current depth: " << current_depth << std::endl;
        // }

        for (Direction direction : kDirections)
        {
            // std::cout << "Entered loop" << std::endl;
            // std::cout << "wall_map size: " << house.wall_map.size() << std::endl;

            Position parent_position = path_tree.getPosition(parent_index);
            Position child_position = Position::computePosition(parent_position, direction);

            // std::cout << "before is_navigable" <<  std::endl;
            // std::cout << "wall_map size: " << house.wall_map.size() << std::endl;
            bool is_navigable = house.wall_map.contains(child_position) && !house.wall_map.at(child_position);
            // std::cout << "after is_navigable" << std::endl;
            bool reached_max_depth = path_tree.getDepth(parent_index) >= max_depth;

            if (!is_navigable || reached_max_depth)
            {
                continue;
            }

            // std::cout << "Parent index: " << parent_index << std::endl;
            // std::cout << "wall_map size: " << house.wall_map.size() << std::endl;

            std::optional<std::size_t> child_index = path_tree.insertChild(parent_index, direction, child_position, isToDoPosition(child_position));
            if (!child_index.has_value())
            {
                continue;
            }

            // std::cout << "Inserted child at index: " << child_index.value() << std::endl;
            // std::cout << "wall_map size: " << house.wall_map.size() << std::endl;

            if (found_criteria(child_position))
            {
                max_depth = std::min(max_depth, path_tree.getDepth(child_index.value()));
                path_tree.registerEndNode(child_index.value());
            } else 
            {
                index_queue.push(child_index.value());
            }

            // std::cout << "End loop" << std::endl;
            // std::cout << "wall_map size: " << house.wall_map.size() << std::endl;
        }
    }

    print("buildPathTree end");
    //std::cout << "buildPathTree path_end_index.has_value(): " << path_end_index.has_value() << std::endl;

    return path_tree.getBestEndNodeIndex();
}

bool BaseAlgorithm::getPathByFoundCriteria(const Position& start_position,
                                           std::deque<Direction>& path,
                                           std::function<bool(const Position&)> const & found_criteria,
                                           std::size_t max_length)
{
    PathTree path_tree;

    // std::cout << "Entered getPathByFoundCriteria" << std::endl;

    std::size_t root_index = path_tree.insertRoot(start_position);

    auto path_end_index = buildPathTree(path_tree, max_length, root_index, found_criteria);

    // std::cout << "getPathByFoundCriteria path_end_index.has_value(): " << path_end_index.has_value() << std::endl;
    if (!path_end_index.has_value())
    {
        return false;
    }

    // Reconstruct found path (backtracing)
    std::size_t current_index = path_end_index.value();
    while (path_tree.hasParent(current_index))
    {
        path.push_front(path_tree.getDirection(current_index));
        current_index = path_tree.getParentIndex(current_index);
    }

    return true;
}

bool BaseAlgorithm::getPathByFoundCriteria(const Position& start_position,
                                           std::deque<Direction>& path,
                                           std::function<bool(const Position&)> const & found_criteria)
{
    return getPathByFoundCriteria(start_position, path, found_criteria, getMaxStepsLeftTillReturnToStation());
}

bool BaseAlgorithm::getPathToNearestTodo(const Position& start_position, std::deque<Direction>& path, std::size_t max_length)
{
    return getPathByFoundCriteria(start_position,
                                  path,
                                  [this](const Position& position)
                                  { return isToDoPosition(position); },
                                  max_length
    );
}

bool BaseAlgorithm::getPathToNearestTodo(const Position& start_position, std::deque<Direction>& path)
{
    return getPathByFoundCriteria(start_position,
                                  path,
                                  [this](const Position& position)
                                  { return isToDoPosition(position); }
    );
}

bool BaseAlgorithm::getPathToPosition(const Position& start_position,
                                  const Position& target_position,
                                  std::deque<Direction>& path,
                                  std::size_t max_length)
{
    return getPathByFoundCriteria(start_position,
                                  path,
                                  [target_position](const Position& position)
                                  { return target_position == position; },
                                  max_length
    );
}

bool BaseAlgorithm::getPathToPosition(const Position& start_position,
                                  const Position& target_position,
                                  std::deque<Direction>& path)
{
    return getPathByFoundCriteria(start_position,
                                  path,
                                  [target_position](const Position& position)
                                  { return target_position == position; }
    );
}

bool BaseAlgorithm::getPathToStation(std::deque<Direction>& path)
{
    return getPathToPosition(current_tile.position, kDockingStationPosition, path);
}

void BaseAlgorithm::sampleWallSensor()
{
    for (Direction direction : kDirections)
    {
        Position position = Position::computePosition(current_tile.position, direction);

        if (house.wall_map.contains(position))
        {
            continue;
        }

        bool is_wall = walls_sensor.value()->isWall(direction);
        house.wall_map[position] = is_wall;

        if (is_wall)
        {
            continue;
        }

        house.todo_positions.insert(position);
    }

    house.wall_map[current_tile.position] = false;
}

void BaseAlgorithm::sampleDirtSensor()
{
    current_tile.dirt_level = dirt_sensor.value()->dirtLevel();

    if (current_tile.dirt_level > 0)
    {
        house.todo_positions.insert(current_tile.position);
    }

    else
    {
        house.todo_positions.erase(current_tile.position);
    }
}

bool BaseAlgorithm::enoughStepsLeftToClean()
{
    std::deque<Direction> path;
    bool is_found = getPathToNearestTodo(current_tile.position, path);
    if (is_found)
    {
        /*
         * Cleaning Cost := Reaching dirty position cost +
         *                  Cleaning 1 tile cost +
         *                  Returning back cost
         */
        std::size_t cleaning_cost = 2 * getPathDistance(path) + 1;
        if (cleaning_cost >= total_steps_left)
        {
            return false;
        }
    }

    return true;
}

std::size_t BaseAlgorithm::getMaxReachableDistance() const
{
    std::size_t max_possible_steps = std::min(battery.full_capacity, max_steps.value());
    if (0 == max_possible_steps)
    {
        return 0;
    }

    /*
     * Number of Steps := Distance (to position) +
     *                    1        (minimal cleaning cost) +
     *                    Distance (back to home)
     */
    std::size_t max_reachable_distance = (max_possible_steps - 1) / 2;
    return max_reachable_distance;
}

bool BaseAlgorithm::isCleanedAllReachable()
{
    std::deque<Direction> found_path;
    // std::cout << "Entered isCleanedAllReachable" << std::endl;
    bool is_found = getPathToNearestTodo(kDockingStationPosition, found_path, total_steps_left);
    // std::cout << "isCleanedAllReachable is_found: " << is_found << std::endl;
    if (!is_found)
    {
        return true;
    }

    if (getPathDistance(found_path) > getMaxReachableDistance())
    {
        return true;
    }

    return false;
}

bool BaseAlgorithm::isValidTargetPath(const std::deque<Direction>& target_path)
{
    Position position = current_tile.position;
    std::size_t steps_to_position = 0;

    for (Direction direction : target_path)
    {
        position = Position::computePosition(position, direction);

        std::deque<Direction> path_from_target_to_station;
        bool found_path_to_station = getPathToPosition(position, kDockingStationPosition, path_from_target_to_station, total_steps_left);
        if (!found_path_to_station)
        {
            throw std::runtime_error("Simulator cannot find path from back to the docking station!");
        }

        steps_to_position++;

        std::size_t total_steps_required = steps_to_position + 1 + path_from_target_to_station.size();

        if (isToDoPosition(position) && total_steps_required <= getMaxStepsLeftTillReturnToStation())
        {
            return true;
        }
    }

    return false;
}

Step BaseAlgorithm::decideNextStep()
{
    std::deque<Direction> path_to_station;
    // std::cout << "decideNextStep enter" << std::endl;
    bool is_found = getPathToStation(path_to_station);
    if (!is_found)
    {
        throw std::runtime_error("Simulator cannot find path back to the docking station!");
    }

    // std::cout << "Found path to station" << std::endl;

    std::size_t station_distance = getPathDistance(path_to_station);

    // std::cout << "Found distance to station: " << station_distance << std::endl;
    bool is_cleaned_all_reachable = isCleanedAllReachable();

    // std::cout << "decideNextStep is_cleaned_all_reachable: " << is_cleaned_all_reachable << std::endl;
    if (shouldFinish(is_cleaned_all_reachable))
    {
        return Step::Finish;
    }

    // std::cout << "decideNextStep shouldKeepCharging: " << shouldKeepCharging() << std::endl;

    if (shouldKeepCharging())
    {
        return Step::Stay;
    }

    // std::cout << "decideNextStep isTooLowBatteryToStay: " << isTooLowBatteryToStay(station_distance) << std::endl;

    if (isTooLowBatteryToStay(station_distance) || is_cleaned_all_reachable)
    {
        return getPathNextStep(path_to_station);
    }

    // std::cout << "decideNextStep isCurrentPositionDirty: " << isCurrentPositionDirty() << std::endl;

    if (isCurrentPositionDirty())
    {
        return Step::Stay;
    }

    // std::cout << "decideNextStep isTooLowBatteryToGetFurther: " << isTooLowBatteryToGetFurther(station_distance) << std::endl;

    if (isTooLowBatteryToGetFurther(station_distance))
    {
        return getPathNextStep(path_to_station);
    }

    // std::cout << "decideNextStep enoughStepsLeftToClean: " << enoughStepsLeftToClean() << std::endl;

    std::deque<Direction> path_to_next_target;
    is_found = getPathToNextTarget(current_tile.position, path_to_next_target);

    // std::cout << "decideNextStep is_found: " << is_found << std::endl;

    // If there's no path to a TODO position - go to station
    if (!is_found || !isValidTargetPath(path_to_next_target))
    {
        if (current_tile.position == kDockingStationPosition)
        {
            return Step::Finish;
        }

        return getPathNextStep(path_to_station);
    }

    // std::cout << "decideNextStep path_to_next_target.size(): " << path_to_next_target.size() << std::endl;

    return getPathNextStep(path_to_next_target);
}

void BaseAlgorithm::move(Step step)
{
    if (Step::Finish == step)
    {
        return;
    }

    safeDecreaseStepsLeft();

    if (Step::Stay == step)
    {
        return;
    }

    Direction direction = static_cast<Direction>(step); // Safe due to prior checks
    current_tile.position = Position::computePosition(current_tile.position, direction);
}

void BaseAlgorithm::setMaxSteps(std::size_t max_steps)
{
    this->max_steps = max_steps;
    total_steps_left = max_steps;
}

void BaseAlgorithm::setWallsSensor(const WallsSensor& walls_sensor)
{
    this->walls_sensor = &walls_sensor;
}

void BaseAlgorithm::setDirtSensor(const DirtSensor& dirt_sensor)
{
    this->dirt_sensor = &dirt_sensor;
}

void BaseAlgorithm::setBatteryMeter(const BatteryMeter& battery_meter)
{
    this->battery_meter = &battery_meter;
    battery.full_capacity = battery_meter.getBatteryState();
}

Step BaseAlgorithm::nextStep()
{
    assertAllInitialied();

    sampleSensors();

    Step step = decideNextStep();
    // std::cout << "Moving nextStep step" << std::endl;
    move(step);

    return step;
}
