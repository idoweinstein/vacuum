#include "house.h"

#include <stdexcept>

#include "robot_logger.h"

void House::setTotalDirtCount()
{
    for (const auto& row : dirt_map)
    {
        for (unsigned int dirt : row)
        {
            total_dirt_count += dirt;
        }
    }
}

House::House(const std::vector<std::vector<bool>>& wall_map,
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
bool House::isOutOfBounds(const std::vector<std::vector<T>>& map, const Position& position)
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

template bool House::isOutOfBounds<bool>(const std::vector<std::vector<bool>>& wall_map, const Position& position);
template bool House::isOutOfBounds<unsigned int>(const std::vector<std::vector<unsigned int>>& dirt_map, const Position& position);

int House::dirtLevel() const
{
    if (isOutOfBounds(dirt_map, current_position))
    {
        throw std::out_of_range("Robot sampled dirt level outside of the house grid!");
    }

    return dirt_map[current_position.first][current_position.second];
}

void House::cleanCurrentPosition()
{
    if (dirtLevel() == 0) // dirtLevel() calls isOutOfBounds() already
    {
        return;
    }

    dirt_map[current_position.first][current_position.second] -= kDirtCleaningUnit;
    total_dirt_count -= kDirtCleaningUnit;
}

bool House::isWall(Direction direction) const
{
    Position suggested_position = Position::computePosition(current_position, direction);

    if (isOutOfBounds(wall_map, suggested_position))
    {
        // Off-grid positions are considered a wall
        return true;
    }

    return wall_map[suggested_position.first][suggested_position.second];
}

void House::move(Step step)
{
    if (Step::Stay == step || Step::Finish == step) {
        return;
    }

    Direction direction = static_cast<Direction>(step);

    if (isWall(direction))
    {
        throw std::runtime_error("Cannot move to wall");
    }

    current_position = Position::computePosition(current_position, direction);
}
