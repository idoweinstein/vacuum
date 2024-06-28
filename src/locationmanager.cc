#include "locationmanager.h"
#include "robotlogger.h"

#include <map>
#include <stdexcept>

LocationManager::LocationManager(std::vector<std::vector<bool>>& wall_map,
                                 std::vector<std::vector<unsigned int>>& dirt_map,
                                 Position docking_station_position)
                                 : wall_map(wall_map),
                                   dirt_map(dirt_map),
                                   current_position(docking_station_position),
                                   docking_station_position(docking_station_position),
                                   total_dirt_count(0)
{

    for (std::vector<unsigned int> row : dirt_map)
    {
        /* Calculate total dirt count */
        for (unsigned int dirt : row)
        {
            total_dirt_count += dirt;
        }
    }
}

template <type T>
bool LocationManager::isOutOfBounds(std::vector<std::vector<T>>& map, Position position)
{
    if (position.first < 0 || position.second < 0)
    {
        return true;
    }

    if (position.first >= map.size())
    {
        return true;
    }

    if (position.second >= map[position.first].size())
    {
        return true
    }

    return false;
}

unsigned int LocationManager::getDirtLevel() const
{
    RobotLogger& logger = RobotLogger::getInstance();

    if (isOutOfBounds(dirt_map, current_position))
    {
        logger.logWarning("Robot sampled dirt level in out of bound location!");
        return 0;
    }
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
    Position suggested_position = Position::computePosition(current_position, direction);
    if (isOutOfBounds(wall_map, suggested_position))
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