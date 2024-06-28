#ifndef VACUUM_LOCATIONMANAGER_H_
#define VACUUM_LOCATIONMANAGER_H_

#include <vector>
#include <utility>

#include "batterysensor.h"
#include "direction.h"
#include "dirtsensor.h"
#include "position.h"

class LocationManager : public BatterySensor, public DirtSensor
{
    std::vector<std::vector<bool>> wall_map;
    std::vector<std::vector<unsigned int>> dirt_map;
    Position current_position;
    Position docking_station_position;
    unsigned int total_dirt_count;

    virtual bool isOutOfBounds(Position position);

public:
    LocationManager(std::vector<std::vector<bool>>& wall_map, std::vector<std::vector<unsigned int>>& dirt_map, Position docking_station_position);
    virtual int getTotalDirtCount() const
    {
        return total_dirt_count;
    }
    virtual void cleanCurrentPoisition();
    virtual void move(Direction);
    virtual bool isInDockingStation() const { return current_position == docking_station_position; }
    virtual bool isFinished() const { return total_dirt_count == 0; }
    virtual unsigned int getDirtLevel() const;
    virtual Position getCurrentPosition() const { return current_position; }
    virtual bool isWall(Direction) const;
};

#endif /* VACUUM_LOCATIONMANAGER_H_ */