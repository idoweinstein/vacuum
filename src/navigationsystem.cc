#include <map>
#include <cmath>
#include <queue>
#include <deque>
#include <stdexcept>
#include <unordered_set>

#include "navigationsystem.h"

NavigationSystem::NavigationSystem(BatterySensor& battery_sensor, DirtSensor& dirt_sensor, WallSensor& wall_sensor)
    : current_position(0, 0), battery_sensor(battery_sensor), dirt_sensor(dirt_sensor), wall_sensor(wall_sensor)
{
    /* Update current location (docking station) as non-wall */
    wall_map[current_position] = false;
}

int NavigationSystem::performBFS(PathTree& path_tree, unsigned int start_index, std::function<bool(Position)> found_criteria)
{
    std::unordered_set<Position> visited_positions;
    std::queue<unsigned int> index_queue;
    static const Direction directions[] = {
        Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST
    };

    // If current position satisfies found_criteria - Return empty path
    if (found_criteria(path_tree.getPosition(start_index))) 
    {
        return start_index;
    }

    index_queue.push(start_index);

    /* Perform BFS */
    while (!index_queue.empty())
    {
        unsigned int parent_index = index_queue.front();
        index_queue.pop();

        for (Direction direction : directions)
        {
            Position parent_position = path_tree.getPosition(parent_index);
            Position child_position = Position::computePosition(parent_position, direction);

            bool is_visited = visited_positions.contains(child_position);
            bool is_navigable = wall_map.contains(child_position) && !wall_map[child_position];

            if (is_visited || !is_navigable)
            {
                continue;
            }

            unsigned int child_index = path_tree.insertChild(parent_index, direction, child_position);
            index_queue.push(child_index);
            visited_positions.insert(child_position);

            if (found_criteria(child_position))
            {
                return child_index; // Node index of path end
            }
        }
    }

    return kNotFound;
}

bool NavigationSystem::getPathByFoundCriteria(std::deque<Direction>& path, std::function<bool(Position)> found_criteria)
{
    PathTree path_tree;

    unsigned int root_index = path_tree.insertRoot(current_position);

    int path_end_index = performBFS(path_tree, root_index, found_criteria);
    if (path_end_index == kNotFound)
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
    static const Direction directions[] = {
        Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST};

    /* Map walls around */
    for (Direction direction : directions) {
        Position position = Position::computePosition(current_position, direction);

        if (wall_map.contains(position)) {
            continue;
        }

        bool is_wall = wall_sensor.isWall(direction);
        wall_map[position] = is_wall;

        if (is_wall) {
            continue;
        }

        todo_positions.insert(position);
    }
}

void NavigationSystem::getSensorsInfo(unsigned int& dirt_level, float& battery_steps)
{
    mapWallsAround();

    /* Update dirt level */
    dirt_level = dirt_sensor.getDirtLevel();
    if (dirt_level > 0)
    {
        todo_positions.insert(current_position);
    }

    /* Update battery level */
    battery_steps = battery_sensor.getCurrentAmount();
    if (battery_steps <= 0)
    {
        throw std::runtime_error("Battery is empty");
    }
}

Direction NavigationSystem::decideNextStep(unsigned int dirt_level, float battery_steps)
{
    std::deque<Direction> path_to_station;
    (void) getPathToStation(path_to_station); // TODO: Handle the case that path back home was not found...

    /* If there's not enough battery, go to station */
    if (battery_steps <= getPathDistance(path_to_station))
    {
        return getPathNextStep(path_to_station);
    }

    /* If the entire map is explored, go to station */
    if (todo_positions.empty())
    {
        /* Go to docking station */
        return getPathNextStep(path_to_station);
    }

    /* If the current position is dirty, stay to clean it */
    if (dirt_level > 0)
    {    
        return Direction::STAY;
    }

    /* If going one step further will cause the battery
       to drain before reaching the station, go to station */
    if (battery_steps <= 1 + getPathDistance(path_to_station))
    {
        return getPathNextStep(path_to_station);
    }

    std::deque<Direction> path_to_nearest_todo;
    bool is_found = getPathToNearestTodo(path_to_nearest_todo);
    if (!is_found) // No path to a dirty block found
    {
        return getPathNextStep(path_to_station);
    }

    return getPathNextStep(path_to_nearest_todo);
}

Direction NavigationSystem::suggestNextStep()
{
    unsigned int dirt_level = 0;
    float battery_steps = 0;

    getSensorsInfo(dirt_level, battery_steps);

    return decideNextStep(dirt_level, battery_steps);
}

void NavigationSystem::move(Direction direction)
{
    current_position = Position::computePosition(current_position, direction);

    if (dirt_sensor.getDirtLevel() == 0)
    {
        todo_positions.erase(current_position);
    }
    /* Update current location (docking station) as non-wall */
    wall_map[current_position] = false;
}