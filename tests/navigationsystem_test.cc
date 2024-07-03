#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "navigationsystem.h"
#include "direction.h"

namespace
{
    class MockWallSensor : public WallSensor
    {
    public:
        MOCK_METHOD(bool, isWall, (Direction direction), (const, override));
    };

    class MockDirtSensor : public DirtSensor
    {
    public:
        MOCK_METHOD(unsigned int, getDirtLevel, (), (const, override));
    };

    class MockBatterySensor : public BatterySensor
    {
    public:
        MOCK_METHOD(float, getCurrentAmount, (), (const, override));
    };

    class NavigationSystemTest : public testing::Test
    {
    protected:
        float battery_level = 100.0;

        MockBatterySensor battery_sensor;
        MockDirtSensor dirt_sensor;
        MockWallSensor wall_sensor;

        NavigationSystem navigation_system;

        NavigationSystemTest() : navigation_system(battery_sensor, dirt_sensor, wall_sensor)
        {
            ON_CALL(battery_sensor, getCurrentAmount())
                .WillByDefault(testing::Invoke([&]()
                {
                    return battery_level--;
                }));

            ON_CALL(dirt_sensor, getDirtLevel())
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
        Direction suggested_direction = navigation_system.suggestNextStep();
        EXPECT_EQ(Direction::FINISH, suggested_direction);
    }

    TEST_F(NavigationSystemTest, dirtyDockingStation)
    {
        EXPECT_CALL(dirt_sensor, getDirtLevel())
            .WillOnce(testing::Return(1));
        
        EXPECT_CALL(wall_sensor, isWall(testing::_))
            .WillRepeatedly(testing::Return(false));

        Direction suggested_direction = navigation_system.suggestNextStep();
        EXPECT_EQ(Direction::STAY, suggested_direction);
    }

    TEST_F(NavigationSystemTest, ReturnToDockingStation)
    {
        setBatteryLevel(5.0);

        bool isAtLimit = false;
        Direction suggested_direction;

        EXPECT_CALL(dirt_sensor, getDirtLevel())
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
            suggested_direction = navigation_system.suggestNextStep();
            EXPECT_EQ(Direction::NORTH, suggested_direction);
            navigation_system.move(suggested_direction);
        }

        isAtLimit = true;
        suggested_direction = navigation_system.suggestNextStep();
        EXPECT_EQ(Direction::STAY, suggested_direction);
        navigation_system.move(suggested_direction);
        
        for (int i = 0; i < 2; i++)
        {
            suggested_direction = navigation_system.suggestNextStep();
            EXPECT_EQ(Direction::SOUTH, suggested_direction);
            navigation_system.move(suggested_direction);
        }
    }

    TEST_F(NavigationSystemTest, TooLowBatteryToGetFurther)
    {
        setBatteryLevel(1.99); // If we will get further, we won't be able to return

        EXPECT_CALL(wall_sensor, isWall(testing::_))
            .WillRepeatedly(testing::Return(false));

        Direction suggested_direction = navigation_system.suggestNextStep();
        EXPECT_EQ(Direction::STAY, suggested_direction);
    }

    TEST_F(NavigationSystemTest, TooLowBatteryToStay)
    {
        setBatteryLevel(2.99); // Whenever we will get further, we won't be able to stay

        EXPECT_CALL(wall_sensor, isWall(testing::_))
            .WillRepeatedly(testing::Return(false));

        // At first, we will get further because we can
        Direction suggested_direction = navigation_system.suggestNextStep();
        EXPECT_NE(Direction::STAY, suggested_direction);
        navigation_system.move(suggested_direction);

        // Then, even though there will be dirt, we will have to return
        EXPECT_CALL(dirt_sensor, getDirtLevel())
            .WillOnce(testing::Return(9));

        suggested_direction = navigation_system.suggestNextStep();
        EXPECT_NE(Direction::STAY, suggested_direction);
    }
}