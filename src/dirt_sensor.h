#ifndef DIRT_SENSOR_H_
#define DIRT_SENSOR_H_

/**
 * @brief The DirtSensor class is an abstract base class for dirt sensors used in a vacuum cleaner.
 *
 * This class provides an interface for getting the dirt level detected by the sensor.
 */
class DirtSensor
{
    public:
        /**
         * @brief Virtual destructor for the DirtSensor class.
         */
        virtual ~DirtSensor() {}

        /**
         * @brief Get the current dirt level detected by the sensor.
         *
         * @return The dirt level as an integer.
         */
        virtual int dirtLevel() const = 0;
};

#endif /* DIRT_SENSOR_H_ */
