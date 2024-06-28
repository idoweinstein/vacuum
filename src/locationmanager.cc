#include "locationmanager.h"

#include <map>
#include <stdexcept>

LocationManager::LocationManager(std::vector<std::vector<bool>>& wall_map, std::vector<std::vector<unsigned int>>& dirt_map, Position docking_station_position) :
    wall_map(wall_map), dirt_map(dirt_map), current_position(docking_station_position), docking_station_position(docking_station_position), total_dirt_count(0)
{

    for (std::vector<unsigned int> row : dirt_map)
    {
        /* Validate map shape */
        if (row.size() != dirt_map[0].size())
        {
            throw std::invalid_argument("Dirt map is not a rectangle");
        }

        /* Calculate total dirt count */
        for (unsigned int dirt : row)
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

bool LocationManager::isOutOfBounds(Position position)
{
    if (position.first < 0 || position.second < 0)
    {
        return true;
    }

    if (position.first >= wall_map.size())
    {
        return true;
    }

    if (position.second >= wall_map[position.first].size())
    {
        return true
    }

    return false;
}

bool LocationManager::isWall(Direction direction) const
{
    Position suggested_position = Position::computePosition(current_position, direction);
    if (isOutOfBounds())
    {
        /* Off-grid positions are considered a wall */
        return true;
    }

    return wall_map[suggested_position.first][suggested_position.second];
}

void LocationManager::move(Direction direction)
{
    if (isWall(direction))
    {
        throw std::runtime_error("Cannot move to wall");
    }

    current_position = Position::computePosition(current_position, direction);
}