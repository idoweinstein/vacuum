#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "navigationsystem.h"
#include "direction.h"

namespace
{
    class MockWallsSensor : public WallsSensor
    {
    public:
        MOCK_METHOD(bool, isWall, (Direction direction), (const, override));
    };

    class MockDirtSensor : public DirtSensor
    {
    public:
        MOCK_METHOD(int, dirtLevel, (), (const, override));
    };

    class MockBatteryMeter : public BatteryMeter
    {
    public:
        MOCK_METHOD(std::size_t, getBatteryState, (), (const, override));
    };

    class NavigationSystemTest : public testing::Test
    {
    protected:
        float battery_level = 100.0;
        const std::size_t max_steps = 10000;

        MockBatteryMeter battery_meter;
        MockDirtSensor dirt_sensor;
        MockWallsSensor wall_sensor;

        NavigationSystem navigation_system;

        NavigationSystemTest() : navigation_system()
        {
            navigation_system.setBatteryMeter(battery_meter);
            navigation_system.setDirtSensor(dirt_sensor);
            navigation_system.setWallsSensor(wall_sensor);
            navigation_system.setMaxSteps(max_steps);

            // TODO: test max steps

            ON_CALL(battery_meter, getBatteryState())
                .WillByDefault(testing::Invoke([&]()
                {
                    return battery_level--;
                }));

            ON_CALL(dirt_sensor, dirtLevel())
                .WillByDefault(testing::Return(0));

            ON_CALL(wall_sensor, isWall(testing::_))
                .WillByDefault(testing::Return(true));
        }

        void setBatteryLevel(float new_battery_level)
        {
            battery_level = new_battery_level;
        }
    };

    TEST_F(NavigationSystemTest, BlockedByWalls)
    {
        Step suggested_step = navigation_system.nextStep();
        EXPECT_EQ(Step::FINISH, suggested_step);
    }

    TEST_F(NavigationSystemTest, dirtyDockingStation)
    {
        EXPECT_CALL(dirt_sensor, dirtLevel())
            .WillOnce(testing::Return(1));
        EXPECT_CALL(wall_sensor, isWall(testing::_))
            .WillRepeatedly(testing::Return(false));
        Step suggested_step = navigation_system.nextStep();
        EXPECT_EQ(Step::STAY, suggested_step);
    }

    TEST_F(NavigationSystemTest, ReturnToDockingStation)
    {
        setBatteryLevel(5.0);

        bool isAtLimit = false;
        Step suggested_step;
        EXPECT_CALL(dirt_sensor, dirtLevel())
            .WillRepeatedly(testing::Invoke([&isAtLimit]()
            {
                if (isAtLimit)
                {
                    return 1;
                }
                return 0;
            }));

        EXPECT_CALL(wall_sensor, isWall(testing::Eq(Direction::EAST)))
            .WillRepeatedly(testing::Return(true));

        EXPECT_CALL(wall_sensor, isWall(testing::Eq(Direction::WEST)))
            .WillRepeatedly(testing::Return(true));

        EXPECT_CALL(wall_sensor, isWall(testing::Eq(Direction::NORTH)))
            .WillRepeatedly(testing::Return(false));

        EXPECT_CALL(wall_sensor, isWall(testing::Eq(Direction::SOUTH)))
            .WillOnce(testing::Return(true))
            .WillRepeatedly(testing::Return(false));

        for (int i = 0; i < 2; i++)
        {
            suggested_step = navigation_system.nextStep();
            EXPECT_EQ(Step::NORTH, suggested_step);
        }

        isAtLimit = true;
        suggested_step = navigation_system.nextStep();
        EXPECT_EQ(Step::STAY, suggested_step);
        for (int i = 0; i < 2; i++)
        {
            suggested_step = navigation_system.nextStep();
            EXPECT_EQ(Step::SOUTH, suggested_step);
        }
    }

    TEST_F(NavigationSystemTest, TooLowBatteryToGetFurther)
    {
        setBatteryLevel(1.99); // If we will get further, we won't be able to return

        EXPECT_CALL(wall_sensor, isWall(testing::_))
            .WillRepeatedly(testing::Return(false));

        Step suggested_step = navigation_system.nextStep();
        EXPECT_EQ(Step::STAY, suggested_step);
    }

    TEST_F(NavigationSystemTest, TooLowBatteryToStay)
    {
        setBatteryLevel(2.99); // Whenever we will get further, we won't be able to stay

        EXPECT_CALL(wall_sensor, isWall(testing::_))
            .WillRepeatedly(testing::Return(false));

        // At first, we will get further because we can
        Step suggested_step = navigation_system.nextStep();
        EXPECT_NE(Step::STAY, suggested_step);

        // Then, even though there will be dirt, we will have to return
        EXPECT_CALL(dirt_sensor, dirtLevel())
            .WillOnce(testing::Return(9));

        suggested_step = navigation_system.nextStep();
        EXPECT_NE(Step::STAY, suggested_step);
    }
}
