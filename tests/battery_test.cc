#include "gtest/gtest.h"

#include <stdexcept>

#include "battery.h"

namespace
{
    TEST(BatteryTest, DischargeAmountSanity)
    {
        Battery battery(10);
        float battery_level = 10.0f;

        for (int i = 0; i < 10; i++)
        {
            EXPECT_EQ(battery_level, battery.getBatteryState());

            battery_level--;
            battery.discharge();
        }

        EXPECT_EQ(0.0f, battery.getBatteryState());

        EXPECT_THROW({
            battery.discharge();
        }, std::range_error);
    }

    TEST(BatteryTest, ChargingTimeSanity)
    {
        float full_amount = 5.0f;
        Battery battery((unsigned int)full_amount);

        for (int i = 0; i < 5; i++)
        {
            battery.discharge();
        }

        EXPECT_EQ(0.0f, battery.getBatteryState());

        for (int i = 0; i < 20; i++)
        {
            EXPECT_LT(battery.getBatteryState(), full_amount);
            battery.charge();
        }

        EXPECT_EQ(full_amount, battery.getBatteryState());

        battery.charge();
        EXPECT_EQ(full_amount, battery.getBatteryState());
    }

    TEST(BatteryTest, NonIntegralCurrentAmount)
    {
        Battery battery(10);

        // Discharge the battery to 5
        for (int i = 0; i < 5; i++)
        {
            battery.discharge();
        }
        EXPECT_EQ(5, battery.getBatteryState());

        // Charge the battery to 5.5
        battery.charge();
        EXPECT_EQ(5, battery.getBatteryState());

        // Charge the battery to 6
        battery.charge();
        EXPECT_EQ(6, battery.getBatteryState());

        // Charge the battery to 6.5
        battery.charge();
        EXPECT_EQ(6, battery.getBatteryState());

        // Discharge the battery to 1.5
        for (int i = 0; i < 5; i++)
        {
            battery.discharge();
        }
        EXPECT_EQ(1, battery.getBatteryState());

        // Charge the battery to 2
        battery.charge();
        EXPECT_EQ(2, battery.getBatteryState());

        // Charge the battery to 2.5
        battery.charge();
        EXPECT_EQ(2, battery.getBatteryState());

        // Discharge the battery to 0.5
        for (int i = 0; i < 2; i++)
        {
            battery.discharge();
        }
        EXPECT_EQ(0, battery.getBatteryState());

        // Try discharging below 0 (-0.5)
        EXPECT_THROW({
            battery.discharge();
        }, std::range_error);

        // Charge the battery to 1
        battery.charge();
        EXPECT_EQ(1, battery.getBatteryState());

        // Discharge the battery to 0
        battery.discharge();
        EXPECT_EQ(0, battery.getBatteryState());

        // Try discharging below 0 (-1)
        EXPECT_THROW({
            battery.discharge();
        }, std::range_error);
    }
}
