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
    std::vector<std::vector<bool>> &wall_map;
    std::vector<std::vector<unsigned int>> &dirt_map;
    UPosition current_position;
    UPosition docking_station_position;
    unsigned int total_dirt_count;

public:
    LocationManager(std::vector<std::vector<bool>> &, std::vector<std::vector<unsigned int>> &, UPosition);
    virtual int getTotalDirtCount() const
    {
        return total_dirt_count;
    }
    virtual void cleanCurrentPoisition();
    virtual void move(Direction);
    virtual bool isInDockingStation() const { return current_position == docking_station_position; }
    virtual bool isFinished() const { return total_dirt_count == 0; }
    virtual unsigned int getDirtLevel() const;
    virtual UPosition getCurrentPosition() const { return current_position; }
    virtual bool isWall(Direction) const;
};

#endif /* VACUUM_LOCATIONMANAGER_H_ */