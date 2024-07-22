#include "algorithm.h"

#include <map>
#include <cmath>
#include <queue>
#include <deque>
#include <stdexcept>
#include <unordered_set>

#include <iostream>

void Algorithm::setMaxSteps(std::size_t max_steps)
{
    this->max_steps = max_steps;
    total_steps_left = max_steps;
}

void Algorithm::setBatteryMeter(const BatteryMeter& battery_meter)
{
    this->battery_meter = &battery_meter;
    battery.full_capacity = battery_meter.getBatteryState();
}

void Algorithm::setDirtSensor(const DirtSensor& dirt_sensor)
{
    this->dirt_sensor = &dirt_sensor;
}

void Algorithm::setWallsSensor(const WallsSensor& walls_sensor)
{
    this->walls_sensor = &walls_sensor;
}

unsigned int Algorithm::scoreByNewTilesDiscovery(Position& position) const
{
    if (house.visited_positions.contains(position))
    {
        return 0;
    }

    unsigned int not_mapped_neighbors_count = 0;

    for (Direction direction : directions)
    {
        Position neighbor_position = Position::computePosition(position, direction);
        if (!house.wall_map.contains(neighbor_position))
        {
            not_mapped_neighbors_count++;
        }
    }

    return not_mapped_neighbors_count;
}

bool Algorithm::performBFS(PathTree& path_tree,
                           unsigned int start_index,
                           const std::function<bool(Position)>& found_criteria) const
{
    std::optional<std::size_t> min_depth_found;
    std::unordered_set<Position> visited_positions;
    std::queue<unsigned int> index_queue;

    // If current position satisfies found_criteria - Return empty path
    if (found_criteria(path_tree.getPosition(start_index)))
    {
        path_tree.markAsPathEnd(start_index);
        return true;
    }

    index_queue.push(start_index);

    // Perform BFS
    while (!index_queue.empty())
    {
        unsigned int parent_index = index_queue.front();
        index_queue.pop();

        if (min_depth_found.has_value() && (path_tree.getDepth(parent_index) >= min_depth_found.value()))
        {
            continue;
        }

        for (Direction direction : directions)
        {
            Position parent_position = path_tree.getPosition(parent_index);
            Position child_position = Position::computePosition(parent_position, direction);

            bool is_visited = visited_positions.contains(child_position);
            bool is_navigable = house.wall_map.contains(child_position) && !house.wall_map.at(child_position);

            if (is_visited || !is_navigable)
            {
                continue;
            }

            unsigned int child_index = path_tree.insertChild(parent_index, direction, child_position, scoreByNewTilesDiscovery(child_position));
            index_queue.push(child_index);
            visited_positions.insert(child_position);

            if (found_criteria(child_position))
            {
                path_tree.markAsPathEnd(child_index);

                if (!min_depth_found.has_value())
                {
                    min_depth_found = path_tree.getDepth(child_index);
                }
            }
        }
    }

    if (min_depth_found.has_value())
    {
        return true;
    }
    return false;
}

unsigned int Algorithm::getBestScorePath(PathTree& path_tree)
{
    return *std::max_element(path_tree.begin(), path_tree.end(), [&path_tree](unsigned int first_index, unsigned int second_index)
                                                                 { return path_tree.getScore(first_index) < path_tree.getScore(second_index); }
    );
}

bool Algorithm::getPathByFoundCriteria(Position start_position, std::deque<Direction>& path, const std::function<bool(Position)>& found_criteria)
{
    PathTree path_tree;

    unsigned int root_index = path_tree.insertRoot(start_position);

    bool is_found = performBFS(path_tree, root_index, found_criteria);
    if (!is_found)
    {
        return false;
    }

    unsigned int path_end_index = getBestScorePath(path_tree);

    // Reconstruct Best Score Found Path
    unsigned int current_index = path_end_index;
    while (path_tree.hasParent(current_index))
    {
        path.push_front(path_tree.getDirection(current_index));
        current_index = path_tree.getParentIndex(current_index);
    }

    return true;
}

bool Algorithm::getPathToNearestTodo(Position start_position, std::deque<Direction>& path)
{
    return getPathByFoundCriteria(start_position,
                                  path,
                                  [this](Position position)
                                  { return house.todo_positions.contains(position); }
    );
}

bool Algorithm::getPathToStation(std::deque<Direction>& path)
{
    return getPathByFoundCriteria(current_tile.position,
                                  path,
                                  [](Position position)
                                  { return position == kDockingStationPosition; }
    );
}

void Algorithm::getWallSensorInfo()
{
    for (Direction direction : directions)
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

void Algorithm::getDirtSensorInfo()
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

void Algorithm::getBatteryMeterInfo()
{
    battery.amount_left = battery_meter.value()->getBatteryState();
}

void Algorithm::getSensorsInfo()
{
    house.visited_positions.insert(current_tile.position);

    getWallSensorInfo();
    getDirtSensorInfo();
    getBatteryMeterInfo();
}

std::size_t Algorithm::getMaxReachableDistance() const
{
    std::size_t max_reachable_steps = std::min(battery.full_capacity, max_steps.value());

    if (0 == max_reachable_steps)
    {
        return 0;
    }

    /*
     * Number of Steps := Distance (to position) +
     *                    1        (minimal cleaning cost) +
     *                    Distance (back to home)
     */
    std::size_t max_reachable_distance = (max_reachable_steps - 1) / 2;
    return max_reachable_distance;
}

bool Algorithm::cleanedAllReachable()
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
    bool is_cleaned_all_reachable = cleanedAllReachable();

    if (shouldFinish(is_cleaned_all_reachable))
    {
        return Step::Finish;
    }

    if (shouldCharge())
    {
        return Step::Stay;
    }

    if (lowBatteryToStay(station_distance) || is_cleaned_all_reachable)
    {
        return getPathNextStep(path_to_station);
    }

    if (currentPositionDirty())
    {
        return Step::Stay;
    }

    if (lowBatteryToGetFurther(station_distance))
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

Step Algorithm::nextStep()
{
    assertAllInitialied();

    getSensorsInfo();

    Step step = decideNextStep();

    move(step);

    return step;
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

    Direction direction = static_cast<Direction>(step);
    current_tile.position = Position::computePosition(current_tile.position, direction);
}
