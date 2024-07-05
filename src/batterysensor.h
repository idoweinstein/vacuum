#ifndef VACUUM_BATTERYSENSOR_H_
#define VACUUM_BATTERYSENSOR_H_

/**
 * @brief The BatterySensor class is a pure abstract base class for battery sensors.
 */
class BatterySensor
{
    public:
        /**
         * @brief Get the current amount of battery power.
         *
         * @return The current amount of battery power as a float value (in steps).
         */
        virtual float getCurrentAmount() const = 0;
};

#endif /* VACUUM_BATTERYSENSOR_H_ */
