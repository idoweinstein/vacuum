#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <cstdlib>

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

    TEST(NavigationSystemTest, blockedByWalls)
    {
        MockBatterySensor battery_sensor;
        MockDirtSensor dirt_sensor;
        MockWallSensor wall_sensor;

        NavigationSystem navigation_system(battery_sensor, dirt_sensor, wall_sensor);

        EXPECT_CALL(battery_sensor, getCurrentAmount())
            .WillRepeatedly(testing::Return(100));

        EXPECT_CALL(dirt_sensor, getDirtLevel())
            .WillRepeatedly(testing::Return(0));

        EXPECT_CALL(wall_sensor, isWall)
            .WillRepeatedly(testing::Return(true));

        Direction suggested_direction = navigation_system.suggestNextStep();
        EXPECT_EQ(Direction::STAY, suggested_direction);
    }
}