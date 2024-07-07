#ifndef VACUUM_BATTERYMETER_H_
#define VACUUM_BATTERYMETER_H_

/**
 * @brief The BatteryMeter class is a pure abstract base class for battery meters (sensors).
 */
class BatteryMeter
{
    public:
        /**
         * @brief Get the current amount of battery power.
         *
         * @return The current amount of battery power as a float value (in steps).
         */
        virtual float getCurrentAmount() const = 0;
};

#endif /* VACUUM_BATTERYMETER_H_ */
