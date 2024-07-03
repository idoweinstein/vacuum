#include "locationmanager.h"

#include <stdexcept>

#include "robotlogger.h"

void LocationManager::setTotalDirtCount()
{
    for (std::vector<unsigned int>& row : dirt_map)
    {
        for (unsigned int& dirt : row)
        {
            total_dirt_count += dirt;
        }
    }
}

LocationManager::LocationManager(const std::vector<std::vector<bool>>& wall_map,
                                 const std::vector<std::vector<unsigned int>>& dirt_map,
                                 const Position& docking_station_position)
                                 : wall_map(wall_map),
                                   dirt_map(dirt_map),
                                   current_position(docking_station_position),
                                   docking_station_position(docking_station_position),
                                   total_dirt_count(0)
{
    setTotalDirtCount();
}

template <typename T>
bool LocationManager::isOutOfBounds(const std::vector<std::vector<T>>& map, const Position& position)
{
    if (position.first < 0 || position.second < 0)
    {
        return true;
    }

    if ((size_t) position.first >= map.size())
    {
        return true;
    }

    if ((size_t) position.second >= map[position.first].size())
    {
        return true;
    }

    return false;
}

template bool LocationManager::isOutOfBounds<bool>(const std::vector<std::vector<bool>>& wall_map, const Position& position);
template bool LocationManager::isOutOfBounds<unsigned int>(const std::vector<std::vector<unsigned int>>& dirt_map, const Position& position);

unsigned int LocationManager::getDirtLevel() const
{
    if (isOutOfBounds(dirt_map, current_position))
    {
        throw std::out_of_range("Robot sampled dirt level outside of the house grid!");
    }

    return dirt_map[current_position.first][current_position.second];
}

void LocationManager::cleanCurrentPosition()
{
    if (getDirtLevel() == 0) // getDirtLevel() calls isOutOfBounds() already
    {
        return;
    }

    dirt_map[current_position.first][current_position.second] -= kDirtCleaningUnit;
    total_dirt_count -= kDirtCleaningUnit;
}

bool LocationManager::isWall(Direction direction) const
{
    Position suggested_position = Position::computePosition(current_position, direction);

    if (isOutOfBounds(wall_map, suggested_position))
    {
        // Off-grid positions are considered a wall
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
