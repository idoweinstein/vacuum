#ifndef VACUUM_BATTERYCONTROLLER_H_
#define VACUUM_BATTERYCONTROLLER_H_

#include "batterymeter.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

/**
 * @brief The BatteryController class represents a controller for a battery.
 *
 * It inherits from the BatteryMeter class and provides functionality to charge and discharge the battery.
 */
class BatteryController : public BatteryMeter
{
    static constexpr const float kStepsToFullAmount = 20.0f; // Charging rate (in steps).
    static constexpr const float kDischargeUnit = 1.0f;      // Discharging rate (in steps).

    const float full_amount;                                 // Full capacity of the battery (in steps).
    float current_amount;                                    // Remaining capacity of the battery (in steps).

public:
    // Disable copy constructor and assignment operator.
    BatteryController(const BatteryController&) = delete;
    BatteryController& operator=(const BatteryController&) = delete;

    /**
     * @brief Constructs a new BatteryController object with the specified full amount of the battery.
     *
     * @param full_amount The full amount of the battery (in steps).
     */
    explicit BatteryController(std::size_t full_amount) : full_amount((float)full_amount), current_amount(full_amount) {}

    /**
     * @brief Gets the current remaining battery capacity.
     *
     * @return The current remaining capacity (in steps).
     */
    virtual std::size_t getBatteryState() const
    {
        return floor(current_amount);
    }

    /**
     * @brief Charges the battery by increasing the current amount.
     *
     * The current amount is increased by 5% of the full amount.
     */
    virtual void charge()
    {
        float updated_amount = current_amount + full_amount / kStepsToFullAmount;
        current_amount = std::min(updated_amount, full_amount);
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
        float updated_amount = current_amount - kDischargeUnit;

        if (updated_amount < 0)
        {
            throw std::range_error("Battery is Empty!");
        }

        current_amount = updated_amount;
    }
};

#endif /* VACUUM_BATTERYCONTROLLER_H_ */
