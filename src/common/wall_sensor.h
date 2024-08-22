#ifndef WALL_SENSOR_H_
#define WALL_SENSOR_H_

#include "common/enums.h"

/**
 * @brief The WallsSensor is an abstract class representing a sensor that detects walls in different directions.
 */
class WallsSensor
{
    public:
        /**
         * @brief Virtual destructor for the WallsSensor class.
         */
        virtual ~WallsSensor() {}

        /**
         * @brief Checks if there is a wall in the specified direction.
         * @param direction The direction to check for a wall.
         * @return true if there is a wall in the specified direction, false otherwise.
         */
        virtual bool isWall(Direction direction) const = 0;
};

#endif /* WALL_SENSOR_H_ */
