#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>

#include "enum_operators.h"
#include "algorithm.h"
#include "position.h"
#include "enums.h"

namespace
{
    class MockWallsSensor : public WallsSensor
    {
    public:
        MockWallsSensor() = default;

        MOCK_METHOD(bool, isWall, (Direction direction), (const, override));
    };

    class MockDirtSensor : public DirtSensor
    {
    public:
        MockDirtSensor() = default;

        MOCK_METHOD(int, dirtLevel, (), (const, override));
    };

    class MockBatteryMeter : public BatteryMeter
    {
    public:
        MockBatteryMeter() = default;

        MOCK_METHOD(std::size_t, getBatteryState, (), (const, override));
    };

    class AlgorithmTest: public testing::Test
    {
    protected:
        float battery_level = 100.0;
        std::size_t max_steps = 1000;
        Position current_position = {0, 0};

        MockBatteryMeter battery_meter;
        MockDirtSensor dirt_sensor;
        MockWallsSensor wall_sensor;

        Algorithm algorithm;

        AlgorithmTest() : battery_meter(), dirt_sensor(), wall_sensor(), algorithm()
        {
            algorithm.setBatteryMeter(battery_meter);
            algorithm.setDirtSensor(dirt_sensor);
            algorithm.setWallsSensor(wall_sensor);
            algorithm.setMaxSteps(max_steps);

            ON_CALL(battery_meter, getBatteryState())
                .WillByDefault(testing::Invoke([&]()
                {
                    return battery_level;
                }));

            ON_CALL(dirt_sensor, dirtLevel())
                .WillByDefault(testing::Return(0));

            ON_CALL(wall_sensor, isWall(testing::_))
                .WillByDefault(testing::Return(true));
        }

        void setBatteryLevel(float new_current_battery_level, float new_full_battery_level)
        {
            EXPECT_CALL(battery_meter, getBatteryState())
                .WillOnce(testing::Invoke([&]()
                {
                    return new_full_battery_level;
                }))
                .WillRepeatedly(testing::Invoke([&]()
                {
                    return battery_level;
                }));

            algorithm.setBatteryMeter(battery_meter);

            battery_level = new_current_battery_level;
        }

        void setMaxSteps(std::size_t new_max_steps)
        {
            algorithm.setMaxSteps(new_max_steps);
            max_steps = new_max_steps;
        }

        void setStartingPosition(Position starting_position)
        {
            current_position = starting_position;
        }

        bool isAtDockingStation()
        {
            return Position(0, 0) == current_position;
        }

        void setIsWall(bool is_wall, Direction direction)
        {
            EXPECT_CALL(wall_sensor, isWall(testing::Eq(direction)))
                .WillRepeatedly(testing::Return(is_wall));
        }

        void setAllIsWall(bool is_wall)
        {
            EXPECT_CALL(wall_sensor, isWall(testing::_))
                .WillRepeatedly(testing::Return(is_wall));
        }

        void setOnceIsWall(bool is_wall, Direction direction)
        {
            EXPECT_CALL(wall_sensor, isWall(testing::Eq(direction)))
                .WillOnce(testing::Return(is_wall))
                .WillRepeatedly(testing::Return(!is_wall));
        }

        void setOnceDirtLevel(unsigned int dirt_level)
        {
            EXPECT_CALL(dirt_sensor, dirtLevel())
                .WillOnce(testing::Return(dirt_level));
        }

        void assertNextStep(Step expected_step)
        {
            Step suggested_step = algorithm.nextStep();

            if (Step::Finish == expected_step)
            {
                EXPECT_TRUE(isAtDockingStation());
            }

            if (Step::Stay == suggested_step && isAtDockingStation())
            {
                battery_level++;
            }

            else
            {
                battery_level--;
            }

            if (Step::Stay != suggested_step && Step::Finish != suggested_step)
            {
                current_position = Position::computePosition(current_position, (Direction) suggested_step);
                std::cout << "Step Took = " << suggested_step << std::endl;
                std::cout << "Current Position = (" << current_position.first << "," << current_position.second << ")" << std::endl;
                std::cout << "Battery Level = " << battery_level << std::endl;
            }
            EXPECT_EQ(expected_step, suggested_step);
        }
    };



    TEST_F(AlgorithmTest, BlockedByWalls)
    {
        assertNextStep(Step::Finish);
    }

    TEST_F(AlgorithmTest, DirtyDockingStation)
    {
        setOnceDirtLevel(1);

        setAllIsWall(false);

        assertNextStep(Step::Stay);
    }

    TEST_F(AlgorithmTest, MinimalBatteryToCleanOnce)
    {
        setBatteryLevel(5.0, 5.0);
        setMaxSteps(5);

        bool isAtLimit = false;
        EXPECT_CALL(dirt_sensor, dirtLevel())
            .WillRepeatedly(testing::Invoke([&isAtLimit]()
            {
                if (isAtLimit)
                {
                    return 9;
                }
                return 0;
            }));

        setIsWall(true, Direction::East);
        setIsWall(true, Direction::West);
        setIsWall(false, Direction::North);
        EXPECT_CALL(wall_sensor, isWall(testing::Eq(Direction::South)))
            .WillOnce(testing::Return(true))
            .WillRepeatedly(testing::Return(false));

        for (int i = 0; i < 2; i++)
        {
            assertNextStep(Step::North);
        }

        isAtLimit = true;
        assertNextStep(Step::Stay);

        for (int i = 0; i < 2; i++)
        {
            assertNextStep(Step::South);
        }

        assertNextStep(Step::Finish);
    }

    TEST_F(AlgorithmTest, TooLowBatteryToGetFurther)
    {
        setBatteryLevel(2.9, 5.0);

        EXPECT_CALL(dirt_sensor, dirtLevel())
            .WillOnce(testing::Invoke([&]() { return 0; }))
            .WillRepeatedly(testing::Invoke([&]() { return 1; }));

        EXPECT_CALL(wall_sensor, isWall(testing::Eq(Direction::East)))
            .WillOnce(testing::Return(true))
            .WillRepeatedly(testing::Return(false));

        setIsWall(false, Direction::West);
        setIsWall(true, Direction::North);
        setIsWall(true, Direction::South);

        assertNextStep(Step::Stay);
    }

    TEST_F(AlgorithmTest, TooLowMaxStepsToGetFurther)
    {
        setMaxSteps(1); // If we will get further, we won't be able to return

        setAllIsWall(false);

        // Robot will predict that it cannot get further
        assertNextStep(Step::Finish);
    }

    TEST_F(AlgorithmTest, TooLowBatteryToClean)
    {
        setBatteryLevel(2, 2); // If we will get further, we won't be able to clean

        setAllIsWall(false);

        // Robot will predict that it cannot clean anyway
        assertNextStep(Step::Finish);
    }

    TEST_F(AlgorithmTest, TooLowMaxStepsToClean)
    {
        setMaxSteps(2); // If we will get further, we won't be able to clean

        setAllIsWall(false);

        // Robot will predict that it cannot clean anyway
        assertNextStep(Step::Finish);
    }

    /*
    * There's no reason to keep charging, because until will get enough battery max steps will be < 3.
    */
    TEST_F(AlgorithmTest, UntilChargedWillHaveTooLowMaxSteps)
    {
        setBatteryLevel(0, 20);
        setMaxSteps(5);

        setAllIsWall(false);

        assertNextStep(Step::Finish);
    }

    /*
    * If robot won't leave docking station while charging he will miss a TODO position he could visit before max steps is reached.
    */
    TEST_F(AlgorithmTest, StopChargingToClean)
    {

        setBatteryLevel(3, 100);
        setMaxSteps(3);

        setOnceIsWall(true, Direction::East);
        setIsWall(false, Direction::West);
        setIsWall(true, Direction::North);
        setIsWall(true, Direction::South);

        // Robot will stop charging to clean before max steps reached 
        assertNextStep(Step::West);
        assertNextStep(Step::East);
        assertNextStep(Step::Finish);
    }
}
