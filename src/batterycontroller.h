#ifndef VACUUM_BATTERYCONTROLLER_H_
#define VACUUM_BATTERYCONTROLLER_H_

#include <algorithm>
#include <stdexcept>

#include "batterysensor.h"

/**
 * @brief The BatteryController class represents a controller for a battery.
 *
 * It inherits from the BatterySensor class and provides functionality to charge and discharge the battery.
 */
class BatteryController : public BatterySensor
{
    const unsigned int full_amount;
    float current_amount;

public:
    /**
     * @brief Constructs a new BatteryController object with the specified full amount of the battery.
     *
     * @param full_amount The full amount of the battery (in steps).
     */
    BatteryController(unsigned int full_amount) : full_amount(full_amount), current_amount(full_amount) {}

    /**
     * @brief Gets the current amount of the battery.
     *
     * @return The current amount of the battery (in steps).
     */
    virtual float getCurrentAmount() const
    {
        return current_amount;
    }

    /**
     * @brief Charges the battery by increasing the current amount.
     *
     * The current amount is increased by 5% of the full amount.
     */
    virtual void charge()
    {
        current_amount = std::min(current_amount + full_amount / (float)20, (float)full_amount);
    }

    /**
     * @brief Discharges the battery by decreasing the current amount.
     *
     * If the resulting current amount is less than 0, a std::range_error exception is thrown.
     * Otherwise, the current amount is decreased by 1.
     *
     * @throws std::range_error If the battery is empty.
     */
    virtual void discharge()
    {
        if (current_amount - 1 < 0)
        {
            throw std::range_error("Empty battery");
        }

        current_amount -= 1;
    }
};

#endif /* VACUUM_BATTERYCONTROLLER_H_ */
