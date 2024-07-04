#ifndef VACUUM_WALLSENSOR_H_
#define VACUUM_WALLSENSOR_H_

#include "direction.h"

/**
 * @brief The WallSensor is a pure abstract class representing a sensor that detects walls in different directions.
 */
class WallSensor
{
    public:
        /**
         * @brief Checks if there is a wall in the specified direction.
         * @param direction The direction to check for a wall.
         * @return true if there is a wall in the specified direction, false otherwise.
         */
        virtual bool isWall(Direction direction) const = 0;
};

#endif /* VACUUM_WALLSENSOR_H_ */
