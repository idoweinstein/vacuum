#ifndef VACUUM_BATTERYCONTROLLER_H_
#define VACUUM_BATTERYCONTROLLER_H_

#include <algorithm>
#include <stdexcept>

#include "batterysensor.h"

class BatteryController : public BatterySensor
{
    const unsigned int full_amount;
    float current_amount;

public:
    BatteryController(unsigned int full_amount) : full_amount(full_amount), current_amount(full_amount) {}
    virtual float getCurrentAmount() const
    {
        return current_amount;
    }
    virtual void charge()
    {
        current_amount = std::min(current_amount + full_amount / (float)20, (float)full_amount);
    }
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
