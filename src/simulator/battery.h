#ifndef BATTERY_H_
#define BATTERY_H_

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "common/battery_meter.h"

/**
 * @brief The Battery class represents a robot battery.
 *
 * It inherits from the BatteryMeter class and provides functionality to charge and discharge the battery.
 */
class Battery : public BatteryMeter
{
    static constexpr const float kEmptyBatteryLevel = 0.0f;  // Empty battery level (in steps).
    static constexpr const float kStepsToFullAmount = 20.0f; // Charging duration (in steps).
    static constexpr const float kDischargeUnit = 1.0f;      // Discharging rate (in steps).

    float full_amount;                                       // Full capacity of the battery (in steps).
    float current_amount;                                    // Remaining capacity of the battery (in steps).

public:
    Battery(const Battery& battery) = default;
    Battery& operator=(const Battery& battery) = default;

    Battery() = default;

    /**
     * @brief Constructs a new Battery object with the specified full amount of the battery.
     *
     * @param full_amount The full amount of the battery (in steps).
     */
    explicit Battery(std::size_t full_amount) : full_amount(static_cast<float>(full_amount)), current_amount(full_amount) {}

    /**
     * @brief Determines whether the battry is empty or not.
     * 
     * @return True if battery is empty, false otherwise.
     */
    bool isBatteryExhausted() const
    {
        return kEmptyBatteryLevel == std::floor(current_amount);
    }

    /**
     * @brief Charges the battery by increasing the current amount.
     *
     * The current amount is increased by 5% of the full amount.
     */
    void charge()
    {
        float updated_amount = current_amount + full_amount / kStepsToFullAmount;
        current_amount = std::min(updated_amount, full_amount);
    }

    /**
     * @brief Discharges the battery by decreasing the current amount.
     *
     * If the resulting current amount is less than 0, a std::range_error exception is thrown.
     * Otherwise, the current amount is decreased by a discharge unit (currently 1).
     *
     * @throws std::range_error If the battery is empty.
     */
    void discharge()
    {
        float updated_amount = current_amount - kDischargeUnit;

        if (updated_amount < 0)
        {
            throw std::range_error("Battery is Empty!");
        }

        current_amount = updated_amount;
    }

    /**
     * @brief Gets the current remaining battery capacity.
     *
     * @return The current remaining capacity (in steps).
     */
    std::size_t getBatteryState() const override
    {
        return std::floor(current_amount);
    }
};

#endif /* BATTERY_H_ */
