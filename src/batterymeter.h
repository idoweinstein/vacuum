#ifndef VACUUM_BATTERYMETER_H_
#define VACUUM_BATTERYMETER_H_

/**
 * @brief The BatteryMeter class is a pure abstract base class for battery meters (sensors).
 */
class BatteryMeter
{
    public:
        /**
         * @brief Virtual destructor for the BatteryMeter class.
         */
        virtual ~BatteryMeter() {}

        /**
         * @brief Get the current amount of remaining battery capacity.
         *
         * @return The current amount of remaining capacity (in steps).
         */
        virtual std::size_t getBatteryState() const = 0;
};

#endif /* VACUUM_BATTERYMETER_H_ */
