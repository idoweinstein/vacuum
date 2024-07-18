#include "algorithm.h"

#include <map>
#include <cmath>
#include <queue>
#include <deque>
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

Algorithm::Algorithm()
    : current_position(0, 0)
{}

void Algorithm::setMaxSteps(std::size_t max_steps)
{
    this->max_steps = max_steps;
}

void Algorithm::setBatteryMeter(const BatteryMeter& battery_meter)
{
    this->battery_meter = &battery_meter;
    full_battery = battery_meter.getBatteryState();
}

void Algorithm::setDirtSensor(const DirtSensor& dirt_sensor)
{
    this->dirt_sensor = &dirt_sensor;
}

void Algorithm::setWallsSensor(const WallsSensor& walls_sensor)
{
    this->walls_sensor = &walls_sensor;
}

int Algorithm::performBFS(PathTree& path_tree, unsigned int start_index, const std::function<bool(Position)>& found_criteria) const
{
    std::unordered_set<Position> visited_positions;
    std::queue<unsigned int> index_queue;

    // If current position satisfies found_criteria - Return empty path
    if (found_criteria(path_tree.getPosition(start_index)))
    {
        return start_index;
    }

    index_queue.push(start_index);

    // Perform BFS
    while (!index_queue.empty())
    {
        unsigned int parent_index = index_queue.front();
        index_queue.pop();

        for (Direction direction : directions)
        {
            Position parent_position = path_tree.getPosition(parent_index);
            Position child_position = Position::computePosition(parent_position, direction);

            bool is_visited = visited_positions.contains(child_position);
            bool is_navigable = wall_map.contains(child_position) && !wall_map.at(child_position);

            if (is_visited || !is_navigable)
            {
                continue;
            }

            unsigned int child_index = path_tree.insertChild(parent_index, direction, child_position);
            index_queue.push(child_index);
            visited_positions.insert(child_position);

            if (found_criteria(child_position))
            {
                return child_index; // Index of last path node
            }
        }
    }

    return kNotFound;
}

bool Algorithm::getPathByFoundCriteria(std::deque<Direction>& path, const std::function<bool(Position)>& found_criteria)
{
    PathTree path_tree;

    unsigned int root_index = path_tree.insertRoot(current_position);

    int path_end_index = performBFS(path_tree, root_index, found_criteria);
    if (kNotFound == path_end_index)
    {
        return false;
    }

    unsigned int current_index = path_end_index;
    while (path_tree.hasParent(current_index))
    {
        path.push_front(path_tree.getDirection(current_index));
        current_index = path_tree.getParentIndex(current_index);
    }

    return true;
}

bool Algorithm::getPathToNearestTodo(std::deque<Direction>& path)
{
    return getPathByFoundCriteria(path,
                                  [&](Position position)
                                  { return todo_positions.contains(position); }
    );
}

bool Algorithm::getPathToStation(std::deque<Direction>& path)
{
    return getPathByFoundCriteria(path,
                                  [&](Position position)
                                  { return position == std::make_pair(0, 0); }
    );
}

void Algorithm::getWallSensorInfo()
{
    for (Direction direction : directions)
    {
        Position position = Position::computePosition(current_position, direction);

        if (wall_map.contains(position))
        {
            continue;
        }

        bool is_wall = walls_sensor.value()->isWall(direction);
        wall_map[position] = is_wall;

        if (is_wall)
        {
            continue;
        }

        todo_positions[position] = std::min(todo_positions[position], current_distance + 1);
    }

    wall_map[current_position] = false;
}

int Algorithm::getDirtSensorInfo()
{
    int dirt_level = dirt_sensor.value()->dirtLevel();

    if (dirt_level > 0)
    {
        todo_positions[current_position] = std::min(todo_positions[current_position], current_distance);
    }

    else
    {
        todo_positions.erase(current_position);
    }

    return dirt_level;
}

void Algorithm::getBatteryMeterInfo(std::size_t& remaining_steps_until_charge, std::size_t& remaining_steps_total, bool& is_battery_full) const
{
    std::size_t remaining_battery_capacity = battery_meter.value()->getBatteryState();

    remaining_steps_total = max_steps.value() - steps_taken;
    remaining_steps_until_charge = std::min(remaining_battery_capacity, remaining_steps_total);

    is_battery_full = (remaining_battery_capacity >= full_battery);   
}

void Algorithm::getSensorsInfo(int& dirt_level, std::size_t& remaining_steps_until_charge, std::size_t& remaining_steps_total, bool& is_battery_full)
{
    getWallSensorInfo();

    dirt_level = getDirtSensorInfo();

    getBatteryMeterInfo(remaining_steps_until_charge, remaining_steps_total, is_battery_full);
}

bool Algorithm::cleanedAllReachable(std::size_t remaining_steps_total)
{
    if (todo_positions.empty())
    {
        return true;
    }

    std::pair<Position, std::size_t> closest_todo = *std::min_element(todo_positions.begin(), todo_positions.end(), CompareDistance());
    std::size_t min_todo_distance = closest_todo.second;
    // To clean this position, we need to reach it, Stay, and return back to station
    std::size_t min_cleaning_cost = 1 + 2 * min_todo_distance;

    int steps_left_to_fully_charged = ((float)full_battery / 20) * (full_battery - battery_meter.value()->getBatteryState());
    std::size_t remaining_steps_after_charge = std::max(0, (int)remaining_steps_total - steps_left_to_fully_charged);
    std::size_t max_possible_steps = std::min((std::size_t)full_battery, remaining_steps_after_charge);
    if (min_cleaning_cost > max_possible_steps)
    {
        return true;
    }

    return false;
}

Step Algorithm::decideNextStep(int dirt_level, std::size_t remaining_steps_until_charge, std::size_t remaining_steps_total, bool is_battery_full)
{
    std::deque<Direction> path_to_station;
    bool is_found = getPathToStation(path_to_station);

    if (!is_found)
    {
        throw std::runtime_error("Simulator cannot find path back to the docking station!");
    }

    std::size_t station_distance = getPathDistance(path_to_station);
    if (0 == station_distance)
    {
        current_distance = 0;
    }

    if (shouldFinish(station_distance, remaining_steps_total))
    {
        return Step::Finish;
    }

    if (shouldCharge(station_distance, is_battery_full))
    {
        return Step::Stay;
    }

    if (lowBatteryToStay(station_distance, remaining_steps_until_charge))
    {
        current_distance--;
        return getPathNextStep(path_to_station);
    }

    if (noPositionsLeftToVisit())
    {
        current_distance--;
        return getPathNextStep(path_to_station);
    }

    if (isCurrentPositionDirty(dirt_level))
    {
        return Step::Stay;
    }

    if (lowBatteryToGetFurther(station_distance, remaining_steps_until_charge))
    {
        current_distance--;
        return getPathNextStep(path_to_station);
    }

    std::deque<Direction> path_to_nearest_todo;
    is_found = getPathToNearestTodo(path_to_nearest_todo);

    // If there's no path to a TODO position - go to station
    if (!is_found)
    {
        return getPathNextStep(path_to_station);
    }

    current_distance++;

    return getPathNextStep(path_to_nearest_todo);
}

Step Algorithm::nextStep()
{
    int dirt_level = 0;
    std::size_t remaining_steps_until_charge = 0;
    std::size_t remaining_steps_total = 0;
    bool is_battery_full = false;

    assertAllInitialied();

    getSensorsInfo(dirt_level, remaining_steps_until_charge, remaining_steps_total, is_battery_full);

    Step step = decideNextStep(dirt_level, remaining_steps_until_charge, remaining_steps_total, is_battery_full);

    move(step);

    return step;
}

void Algorithm::move(Step step)
{
    if (Step::Finish == step)
    {
        return;
    }

    steps_taken++;

    if (Step::Stay == step)
    {
        return;
    }

    Direction direction = static_cast<Direction>(step);
    current_position = Position::computePosition(current_position, direction);
}
