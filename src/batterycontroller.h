#ifndef VACUUM_BATTERYCONTROLLER_H_
#define VACUUM_BATTERYCONTROLLER_H_

#include "batterysensor.h"

#include <algorithm>
#include <stdexcept>

class BatteryController : public BatterySensor
{
    static constexpr const float kStepsToFullAmount = 20.0f;
    static constexpr const float kDischargeUnit = 1.0f;

    const float full_amount;
    float current_amount;

public:
    BatteryController(unsigned int full_amount) : full_amount((float)full_amount), current_amount(full_amount) {}

    virtual float getCurrentAmount() const
    {
        return current_amount;
    }

    virtual void charge()
    {
        float updated_amount = current_amount + full_amount / kStepsToFullAmount;
        current_amount = std::min(updated_amount, full_amount);
    }

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
