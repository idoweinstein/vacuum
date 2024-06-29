#ifndef VACUUM_DIRTSENSOR_H_
#define VACUUM_DIRTSENSOR_H_

/**
 * @brief The DirtSensor class is a pure abstract base class for dirt sensors used in a vacuum cleaner.
 *
 * This class provides an interface for getting the dirt level detected by the sensor.
 */
class DirtSensor {
    public:
        /**
         * @brief Get the current dirt level detected by the sensor.
         *
         * @return The dirt level as an unsigned integer.
         */
        virtual unsigned int getDirtLevel() const = 0;
};

#endif /* VACUUM_DIRTSENSOR_H_ */
