#include "locationmanager.h"

#include <map>
#include <stdexcept>

LocationManager::LocationManager(std::vector<std::vector<bool>>& wall_map, std::vector<std::vector<unsigned int>>& dirt_map, std::pair<unsigned int, unsigned int> docking_station_position) :
    wall_map(wall_map), dirt_map(dirt_map), current_position(docking_station_position), docking_station_position(docking_station_position), total_dirt_count(0)
{
    /* Calculate total dirt count */
    for (auto row : dirt_map)
    {
        for (auto dirt : row)
        {
            total_dirt_count += dirt;
        }
    }
}

unsigned int LocationManager::getDirtLevel() const
{
    return dirt_map[current_position.first][current_position.second];
}

void LocationManager::cleanCurrentPoisition()
{
    if (getDirtLevel() == 0)
    {
        return;
    }

    dirt_map[current_position.first][current_position.second] -= 1;
    total_dirt_count -= 1;
}

bool LocationManager::isWall(Direction direction) const
{
    /* TODO: move the following calculation to a new method in
             Position/UPosition */
    static std::map<Direction, std::pair<int, int>> map = {
        {Direction::NORTH, std::make_pair(-1, 0)},
        {Direction::EAST, std::make_pair(0, 1)},
        {Direction::SOUTH, std::make_pair(1, 0)},
        {Direction::WEST, std::make_pair(0, -1)}};

    std::pair<int, int> next_position = std::make_pair(current_position.first + map[direction].first,
                                                       current_position.second + map[direction].second);

    return wall_map[next_position.first][next_position.second];
}

void LocationManager::move(Direction direction)
{
    if (isWall(direction))
    {
        throw std::runtime_error("Cannot move to wall");
    }

    /* TODO: move the following calculation to a new method in
             Position/UPosition */
    static std::map<Direction, std::pair<int, int>> map = {
        {Direction::NORTH, std::make_pair(-1, 0)},
        {Direction::EAST, std::make_pair(0, 1)},
        {Direction::SOUTH, std::make_pair(1, 0)},
        {Direction::WEST, std::make_pair(0, -1)}};

    current_position.first += map[direction].first;
    current_position.second += map[direction].second;
}