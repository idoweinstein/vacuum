#ifndef VACUUM_WALLSENSOR_H_
#define VACUUM_WALLSENSOR_H_

#include "direction.h"

class WallSensor {
    public:
        virtual bool isWall(Direction) const = 0;
};

#endif /* VACUUM_WALLSENSOR_H_ */