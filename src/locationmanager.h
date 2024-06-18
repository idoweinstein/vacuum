#ifndef VACUUM_LOCATIONMANAGER_H_
#define VACUUM_LOCATIONMANAGER_H_

#include <vector>
#include <tuple>

#include "batterysensor.h"
#include "direction.h"
#include "dirtsensor.h"

using namespace std;

class LocationManager : public BatterySensor, public DirtSensor
{
    vector<vector<bool>> &wall_map;
    vector<vector<unsigned int>> &dirt_map;
    tuple<unsigned int, unsigned int> current_position;
    tuple<unsigned int, unsigned int> docking_station_position;
    unsigned int total_dirt_count;

public:
    LocationManager(vector<vector<bool>> &, vector<vector<unsigned int>> &, tuple<unsigned int, unsigned int>);
    virtual int getTotalDirtCount() const
    {
        return total_dirt_count;
    }
    virtual void cleanCurrentPoisition();
    virtual void move(Direction);
    virtual bool isInDockingStation() const { return current_position == docking_station_position; }
    virtual bool isFinished() const { return total_dirt_count == 0; }
    virtual unsigned int getDirtLevel() const;
    virtual bool isWall(Direction) const;
};

#endif /* VACUUM_LOCATIONMANAGER_H_ */