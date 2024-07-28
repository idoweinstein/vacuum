#include "algorithm.h"

#include "algorithm/AlgorithmRegistration.h"

#include <map>
#include <cmath>
#include <queue>
#include <deque>
#include <stdexcept>
#include <unordered_set>

std::optional<std::size_t> Algorithm::performBFS(PathTree& path_tree,
                                                 std::size_t start_index,
                                                 std::function<bool(const Position&)> const & found_criteria) const
{
    std::unordered_set<Position> visited_positions;
    std::queue<std::size_t> index_queue;
    std::optional<std::size_t> path_end_index;

    // If current position satisfies found_criteria - Return empty path
    if (found_criteria(path_tree.getPosition(start_index)))
    {
        path_end_index = start_index;
        return path_end_index;
    }

    index_queue.push(start_index);

    // Perform BFS
    while (!index_queue.empty())
    {
        std::size_t parent_index = index_queue.front();
        index_queue.pop();

        for (Direction direction : kDirections)
        {
            Position parent_position = path_tree.getPosition(parent_index);
            Position child_position = Position::computePosition(parent_position, direction);

            bool is_visited = visited_positions.contains(child_position);
            bool is_navigable = house.wall_map.contains(child_position) && !house.wall_map.at(child_position);

            if (is_visited || !is_navigable)
            {
                continue;
            }

            std::size_t child_index = path_tree.insertChild(parent_index, direction, child_position);
            index_queue.push(child_index);
            visited_positions.insert(child_position);

            if (found_criteria(child_position))
            {
                path_end_index = child_index;
                return path_end_index;
            }
        }
    }

    return path_end_index;
}

bool Algorithm::getPathByFoundCriteria(const Position& start_position,
                                       std::deque<Direction>& path,
                                       std::function<bool(const Position&)> const & found_criteria)
{
    PathTree path_tree;

    std::size_t root_index = path_tree.insertRoot(start_position);

    auto path_end_index = performBFS(path_tree, root_index, found_criteria);
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

bool Algorithm::getPathToNearestTodo(const Position& start_position, std::deque<Direction>& path)
{
    return getPathByFoundCriteria(start_position,
                                  path,
                                  [this](const Position& position)
                                  { return house.todo_positions.contains(position); }
    );
}

bool Algorithm::getPathToStation(std::deque<Direction>& path)
{
    return getPathByFoundCriteria(current_tile.position,
                                  path,
                                  [](const Position& position)
                                  { return kDockingStationPosition == position; }
    );
}

void Algorithm::sampleWallSensor()
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

void Algorithm::sampleDirtSensor()
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

bool Algorithm::enoughStepsLeftToClean()
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

std::size_t Algorithm::getMaxReachableDistance() const
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

bool Algorithm::isCleanedAllReachable()
{
    std::deque<Direction> found_path;
    bool is_found = getPathToNearestTodo(kDockingStationPosition, found_path);
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

Step Algorithm::decideNextStep()
{
    std::deque<Direction> path_to_station;
    bool is_found = getPathToStation(path_to_station);
    if (!is_found)
    {
        throw std::runtime_error("Simulator cannot find path back to the docking station!");
    }

    std::size_t station_distance = getPathDistance(path_to_station);
    bool is_cleaned_all_reachable = isCleanedAllReachable();

    if (shouldFinish(is_cleaned_all_reachable))
    {
        return Step::Finish;
    }

    if (shouldKeepCharging())
    {
        return Step::Stay;
    }

    if (isTooLowBatteryToStay(station_distance) || is_cleaned_all_reachable)
    {
        return getPathNextStep(path_to_station);
    }

    if (isCurrentPositionDirty())
    {
        return Step::Stay;
    }

    if (isTooLowBatteryToGetFurther(station_distance))
    {
        return getPathNextStep(path_to_station);
    }

    std::deque<Direction> path_to_nearest_todo;
    is_found = getPathToNearestTodo(current_tile.position, path_to_nearest_todo);

    // If there's no path to a TODO position - go to station
    if (!is_found)
    {
        return getPathNextStep(path_to_station);
    }

    return getPathNextStep(path_to_nearest_todo);
}

void Algorithm::move(Step step)
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

void Algorithm::setMaxSteps(std::size_t max_steps)
{
    this->max_steps = max_steps;
    total_steps_left = max_steps;
}

void Algorithm::setWallsSensor(const WallsSensor& walls_sensor)
{
    this->walls_sensor = &walls_sensor;
}

void Algorithm::setDirtSensor(const DirtSensor& dirt_sensor)
{
    this->dirt_sensor = &dirt_sensor;
}

void Algorithm::setBatteryMeter(const BatteryMeter& battery_meter)
{
    this->battery_meter = &battery_meter;
    battery.full_capacity = battery_meter.getBatteryState();
}

Step Algorithm::nextStep()
{
    assertAllInitialied();

    sampleSensors();

    Step step = decideNextStep();

    move(step);

    return step;
}

REGISTER_ALGORITHM(Algorithm);
