#include "navigationsystem.h"

#include <map>
#include <cmath>
#include <queue>
#include <deque>
#include <stdexcept>
#include <unordered_set>

NavigationSystem::NavigationSystem()
    : current_position(0, 0)
{
    // Update current location (docking station) as non-wall
    wall_map[current_position] = false;
}

void NavigationSystem::setMaxSteps(std::size_t max_steps) {
    this->max_steps = max_steps;
}

void NavigationSystem::setBatteryMeter(const BatteryMeter& battery_meter) {
    this->battery_meter = &battery_meter;
    full_battery = battery_meter.getBatteryState();
}

void NavigationSystem::setDirtSensor(const DirtSensor& dirt_sensor) {
    this->dirt_sensor = &dirt_sensor;
}

void NavigationSystem::setWallsSensor(const WallsSensor& walls_sensor) {
    this->walls_sensor = &walls_sensor;
}

int NavigationSystem::performBFS(PathTree& path_tree, unsigned int start_index, const std::function<bool(Position)>& found_criteria) const
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

bool NavigationSystem::getPathByFoundCriteria(std::deque<Direction>& path, const std::function<bool(Position)>& found_criteria)
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

bool NavigationSystem::getPathToNearestTodo(std::deque<Direction>& path)
{
    return getPathByFoundCriteria(path,
                                  [&](Position position)
                                  { return todo_positions.contains(position); }
    );
}

bool NavigationSystem::getPathToStation(std::deque<Direction>& path)
{
    return getPathByFoundCriteria(path,
                                  [&](Position position)
                                  { return position == std::make_pair(0, 0); }
    );
}

void NavigationSystem::mapWallsAround()
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

        todo_positions.insert(position);
    }
}

void NavigationSystem::getSensorsInfo(int& dirt_level, std::size_t& reamining_steps_until_charge, std::size_t& remaining_steps_total, bool& is_battery_full)
{
    mapWallsAround();

    dirt_level = dirt_sensor.value()->dirtLevel();
    if (dirt_level > 0)
    {
        todo_positions.insert(current_position);
    } else {
        todo_positions.erase(current_position);
    }

    /* Update current location (docking station) as non-wall */
    wall_map[current_position] = false;

    std::size_t remaining_battery_capacity = battery_meter.value()->getBatteryState();

    remaining_steps_total = max_steps.value() - steps_taken;
    reamining_steps_until_charge = std::min(remaining_battery_capacity, remaining_steps_total);

    is_battery_full = (remaining_battery_capacity >= full_battery);
}

Step NavigationSystem::decideNextStep(int dirt_level, std::size_t remaining_steps_until_charge, std::size_t remaining_steps_total, bool is_battery_full)
{
    std::deque<Direction> path_to_station;
    bool is_found = getPathToStation(path_to_station);
    if (!is_found)
    {
        throw std::runtime_error("Robot cannot find path back to the docking station!");
    }

    // If no allowed steps OR
    // left no dirty accessible places AND we're in docking station - finish cleaning
    if (remaining_steps_total <= 0 || (path_to_station.empty() && todo_positions.empty()))
    {
        return Step::FINISH;
    }

    // If charging - charge until battery is full
    if (path_to_station.empty() && !is_battery_full)
    {
        return Step::STAY;
    }

    // If there's not enough battery - go to station
    if (remaining_steps_until_charge < 1 + getPathDistance(path_to_station))
    {
        return getPathNextStep(path_to_station);
    }

    // If no positions left to visit (visited & cleaned all accessible positions) - go to station
    if (todo_positions.empty())
    {
        return getPathNextStep(path_to_station);
    }

    // If the current position is dirty, stay to clean it
    if (dirt_level > 0)
    {
        return Step::STAY;
    }

    /* If going one step further will cause the battery
       to drain before reaching the station - go to station */
    if (remaining_steps_until_charge < 2 + getPathDistance(path_to_station))
    {
        return getPathNextStep(path_to_station);
    }

    std::deque<Direction> path_to_nearest_todo;
    is_found = getPathToNearestTodo(path_to_nearest_todo);
    // If there's no path to a TODO position - go to station
    if (!is_found)
    {
        return getPathNextStep(path_to_station);
    }

    return getPathNextStep(path_to_nearest_todo);
}

Step NavigationSystem::nextStep()
{
    int dirt_level = 0;
    std::size_t remaining_steps_until_charge = 0;
    std::size_t remaining_steps_total = 0;
    bool is_battery_full = false;

    checkInited();

    getSensorsInfo(dirt_level, remaining_steps_until_charge, remaining_steps_total, is_battery_full);

    Step step = decideNextStep(dirt_level, remaining_steps_until_charge, remaining_steps_total, is_battery_full);

    move(step);

    return step;
}

void NavigationSystem::move(Step step)
{
    if (Step::FINISH != step)
    {
        steps_taken++;
    
    }

    if (Step::STAY != step && Step::FINISH != step)
    {
        Direction direction = static_cast<Direction>(step);
        current_position = Position::computePosition(current_position, direction);
    }
}
