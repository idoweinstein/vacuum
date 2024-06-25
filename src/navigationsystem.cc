#include <cmath>
#include <map>
#include <queue>
#include <stdexcept>
#include <unordered_set>

#include "navigationsystem.h"

NavigationSystem::NavigationSystem(BatterySensor &battery_sensor, DirtSensor &dirt_sensor, WallSensor &wall_sensor)
    : battery_sensor(battery_sensor), dirt_sensor(dirt_sensor), wall_sensor(wall_sensor), current_position(0, 0)
{
    /* Update current location (docking station) as non-wall */
    wall_map[current_position] = false;
}

Direction NavigationSystem::suggestNextStep()
{
    static const Direction directions[] = {
        Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST};

    /* Map walls around */
    for (auto direction : directions) {
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

    /* Dirt level */
    unsigned int dirt_level = dirt_sensor.getDirtLevel();
    if (dirt_level == 0) {
        todo_positions.erase(current_position);
    }

    /* Battery */
    float battery_steps = battery_sensor.getCurrentAmount();

    std::vector<Direction> pathToStation = getPathToStation();

    /* If there's not enough battery, go to station */
    if (battery_steps <= pathToStation.size()) {
        return pathToStation[0];
    }

    /* If the entire map is explored, go to station */
    if (todo_positions.empty()) {
        /* Go to docking station */
        return pathToStation[0];
    }

    /* If the current position is dirty, stay to clean it */
    if (dirt_level > 0) {
        return Direction::STAY;
    }

    /* If going one step further will cause the battery
       to drain before reaching the station, go to station */
    if (battery_steps <= 1 + pathToStation.size()) {
        return pathToStation[0];
    }

    std::vector<Direction>* pathToNearestTodo = getPathToNearestTodo();
    if (pathToNearestTodo == nullptr) {
        /* All TODOs are non-reachable (should not happen) */
        return pathToStation[0];
    }

    Direction nextStep = (*pathToNearestTodo)[0];

    delete pathToNearestTodo;

    return nextStep;
}

void NavigationSystem::move(Direction direction)
{
    current_position = Position::computePosition(current_position, direction);

    /* Update current location (docking station) as non-wall */
    wall_map[current_position] = false;
}

struct node {
    struct node *parent;
    Direction direction;
    Position position;
};

std::vector<Direction>* NavigationSystem::performBFS(Position start_position, std::function<bool(Position)> found)
{
    static const Direction directions[] = {
        Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST};

    std::unordered_set<Position> visited;
    std::queue<struct node *> queue;
    struct node *nearest = nullptr;

    struct node *root = new node;
    root->parent = nullptr;
    root->direction = Direction::STAY;
    root->position = start_position;
    queue.push(root);

    /* Perform BFS */
    while (nearest == nullptr && !queue.empty()) {
        struct node *current = queue.front();
        queue.pop();

        for (auto direction : directions) {
            Position position = Position::computePosition(current->position, direction);

            if (visited.contains(position) || wall_map.contains(position) && wall_map[position]) {
                /* This position is a wall or already has been visited */
                continue;
            }

            struct node *child = new node;

            child->parent = current;
            child->direction = direction;
            child->position = position;
            queue.push(child);

            if (found(child->position)) {
                nearest = child;
                break;
            }
        }
    }

    if (nearest == nullptr) {
        /* No matching node */
        return nullptr;
    }

    /* Reconstruct path */
    std::vector<Direction> *path = new std::vector<Direction>;
    struct node *current = nearest;
    while (current->parent != nullptr)
    {
        path->insert(path->begin(), current->direction);
        current = current->parent;
    }

    /* Free entire tree */
    visited.clear();
    while (!queue.empty())
    {
        struct node *current = queue.front();
        queue.pop();
        if (visited.contains(current->position))
        {
            continue;
        }

        visited.insert(current->position);
        if (current->parent != nullptr)
        {
            queue.push(current->parent);
        }
        delete current;
    }

    return path;
}

std::vector<Direction>* NavigationSystem::getPathToNearestTodo()
{
    return performBFS(current_position, [&](Position position) {
        return todo_positions.contains(position);
    });
}

std::vector<Direction>& NavigationSystem::getPathToStation()
{
    std::vector<Direction>* path = performBFS(current_position, [&](Position position) {
        return position == std::make_pair(0, 0);
    });

    if (path == nullptr) {
        /* Should not happen */
        throw std::runtime_error("No path to station");
    }

    return *path;
}