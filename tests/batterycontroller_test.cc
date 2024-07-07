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

        for (int i = 0; i < 5; i++)
        {
            battery_controller.discharge();
        }

        EXPECT_EQ(5.0f, battery_controller.getBatteryState());

        battery_controller.charge();
        EXPECT_EQ(5.5f, battery_controller.getBatteryState());

        for (int i = 0; i < 5; i++)
        {
            battery_controller.discharge();
        }

        EXPECT_EQ(0.5f, battery_controller.getBatteryState());

        EXPECT_THROW({
            battery_controller.discharge();
        }, std::range_error);
    }
}
