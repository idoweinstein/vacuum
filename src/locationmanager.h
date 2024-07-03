#ifndef VACUUM_LOCATIONMANAGER_H_
#define VACUUM_LOCATIONMANAGER_H_

#include <vector>

#include "dirtsensor.h"
#include "wallsensor.h"
#include "direction.h"
#include "position.h"

class LocationManager : public WallSensor, public DirtSensor
{
    static constexpr const unsigned int kDirtCleaningUnit = 1;

    std::vector<std::vector<bool>> wall_map;
    std::vector<std::vector<unsigned int>> dirt_map;
    Position current_position;
    Position docking_station_position;
    unsigned int total_dirt_count;

    virtual void setTotalDirtCount();
    template <typename T> bool isOutOfBounds(const std::vector<std::vector<T>>& map, const Position& position) const;

public:
    LocationManager(const std::vector<std::vector<bool>>& wall_map,
                    const std::vector<std::vector<unsigned int>>& dirt_map,
                    const Position& docking_station_position);

    virtual int getTotalDirtCount() const
    {
        return total_dirt_count;
    }

    virtual void cleanCurrentPosition();
    virtual void move(Direction);
    virtual bool isInDockingStation() const { return current_position == docking_station_position; }
    virtual bool isFinished() const { return total_dirt_count == 0; }
    virtual unsigned int getDirtLevel() const;
    virtual Position getCurrentPosition() const { return current_position; }
    virtual bool isWall(Direction) const;
};

#endif /* VACUUM_LOCATIONMANAGER_H_ */