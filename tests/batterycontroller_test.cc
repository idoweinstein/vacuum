#include "gtest/gtest.h"

#include <stdexcept>

#include "batterycontroller.h"

namespace
{
    TEST(BatteryControllerTest, DischargeAmountSanity)
    {
        BatteryController battery_controller(10);
        float battery_level = 10.0f;

        for (int i = 0; i < 10; i++)
        {
            EXPECT_EQ(battery_level, battery_controller.getBatteryState());

            battery_level--;
            battery_controller.discharge();
        }

        EXPECT_EQ(0.0f, battery_controller.getBatteryState());

        EXPECT_THROW({
            battery_controller.discharge();
        }, std::range_error);
    }

    TEST(BatteryControllerTest, ChargingTimeSanity)
    {
        float full_amount = 5.0f;
        BatteryController battery_controller((unsigned int)full_amount);

        for (int i = 0; i < 5; i++)
        {
            battery_controller.discharge();
        }

        EXPECT_EQ(0.0f, battery_controller.getBatteryState());

        for (int i = 0; i < 20; i++)
        {
            EXPECT_LT(battery_controller.getBatteryState(), full_amount);
            battery_controller.charge();
        }

        EXPECT_EQ(full_amount, battery_controller.getBatteryState());

        battery_controller.charge();
        EXPECT_EQ(full_amount, battery_controller.getBatteryState());
    }

    TEST(BatteryControllerTest, NonIntegralCurrentAmount)
    {
        BatteryController battery_controller(10);

        // Discharge the battery to 5
        for (int i = 0; i < 5; i++)
        {
            battery_controller.discharge();
        }
        EXPECT_EQ(5, battery_controller.getBatteryState());

        // Charge the battery to 5.5
        battery_controller.charge();
        EXPECT_EQ(5, battery_controller.getBatteryState());

        // Charge the battery to 6
        battery_controller.charge();
        EXPECT_EQ(6, battery_controller.getBatteryState());

        // Charge the battery to 6.5
        battery_controller.charge();
        EXPECT_EQ(6, battery_controller.getBatteryState());

        // Discharge the battery to 1.5
        for (int i = 0; i < 5; i++)
        {
            battery_controller.discharge();
        }
        EXPECT_EQ(1, battery_controller.getBatteryState());

        // Charge the battery to 2
        battery_controller.charge();
        EXPECT_EQ(2, battery_controller.getBatteryState());

        // Charge the battery to 2.5
        battery_controller.charge();
        EXPECT_EQ(2, battery_controller.getBatteryState());

        // Discharge the battery to 0.5
        for (int i = 0; i < 2; i++)
        {
            battery_controller.discharge();
        }
        EXPECT_EQ(0, battery_controller.getBatteryState());

        // Try discharging below 0 (-0.5)
        EXPECT_THROW({
            battery_controller.discharge();
        }, std::range_error);

        // Charge the battery to 1
        battery_controller.charge();
        EXPECT_EQ(1, battery_controller.getBatteryState());

        // Discharge the battery to 0
        battery_controller.discharge();
        EXPECT_EQ(0, battery_controller.getBatteryState());

        // Try discharging below 0 (-1)
        EXPECT_THROW({
            battery_controller.discharge();
        }, std::range_error);
    }
}
