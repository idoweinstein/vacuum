#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <ios>
#include <regex>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>

#include "common/AlgorithmRegistrar.h"
#include "common/enums.h"

#include "simulator/deserializer.h"
#include "simulator/simulator.h"

#include "algorithm/a/greedy_algorithm.h" // TODO: change
#include "algorithm/b/dfs_algorithm.h"

namespace
{
    std::array<AlgorithmFactory, 2> algo_factories = {
        []() { return std::make_unique<GreedyAlgorithm>(); },
        []() { return std::make_unique<DFSAlgorithm>(); }
    };

    class MockAlgorithm : public AbstractAlgorithm
    {
        unsigned long max_steps = 0;
    public:
        MockAlgorithm() = default;

        MOCK_METHOD(void, setWallsSensor, (const WallsSensor&), (override));
        MOCK_METHOD(void, setDirtSensor, (const DirtSensor&), (override));
        MOCK_METHOD(void, setBatteryMeter, (const BatteryMeter&), (override));
        MOCK_METHOD(Step, nextStep, (), (override));

        void setMaxSteps(unsigned long max_steps) override
        {
            this->max_steps = max_steps;
        }

        unsigned long getMaxSteps() const
        {
            return max_steps;
        }
    };

    class SimulatorTest : public testing::TestWithParam<AlgorithmFactory>
    {
        std::unique_ptr<AbstractAlgorithm> algorithm;
        Simulator simulator;
    protected:

        void SetUp(const std::string& input_file)
        {
            algorithm = GetParam()();

            simulator.readHouseFile(input_file);
            simulator.setAlgorithm(algorithm);
            simulator.run();
        }

        SimulationStatistics& getSimulationStatistics()
        {
            return simulator.getSimulationStatistics();
        }
    };

    TEST_P(SimulatorTest, RobotSanity)
    {
        SetUp("inputs/input_sanity.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Make sure robot did 'total_steps_taken' steps, and its first step wasn't Direction::STAY
        std::size_t path_length = statistics.step_history.size();
        if (statistics.step_history.back() == Step::Finish)
        {
            path_length--;
        }
        EXPECT_EQ(statistics.num_steps_taken, path_length);
        EXPECT_NE(Step::Stay, statistics.step_history.at(0));

        // Assert the expected program results (Robot is not dead and cleaned all dirt)
        EXPECT_EQ(Status::Finished, statistics.mission_status);

        EXPECT_TRUE(statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotTrappedDirt)
    {
        SetUp("inputs/input_trappeddirt.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected program results (Robot is not dead, cleaned all ACCESSIBLE dirt, but there's more trapped dirt)
        EXPECT_EQ(Status::Finished, statistics.mission_status);
        EXPECT_LT(0, statistics.dirt_left);

        EXPECT_TRUE(statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotMaze)
    {
        SetUp("inputs/input_maze.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, statistics.mission_status);

        EXPECT_TRUE(statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotMinimalBatteryToComplete)
    {
        SetUp("inputs/input_minbattery.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, statistics.mission_status);

        Step expected_steps[] = {
            Step::East,
            Step::Stay,
            Step::West,
            Step::Finish
        };
        size_t expected_steps_num = 4;

        if (expected_steps_num != statistics.step_history.size())
        {
            FAIL();
            return;
        }

        for (unsigned int i = 0; i < expected_steps_num; i++)
        {
            EXPECT_EQ(expected_steps[i], statistics.step_history.at(i));
        }

        EXPECT_TRUE(statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotTooDistantDirt)
    {
        SetUp("inputs/input_distantdirt.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, statistics.mission_status);

        EXPECT_GE(50, statistics.num_steps_taken);

        EXPECT_EQ(1, statistics.dirt_left);

        EXPECT_TRUE(statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotAllCharacters)
    {
        SetUp("inputs/input_allchars.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, statistics.mission_status);

        EXPECT_TRUE(statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotNoHouse)
    {
        EXPECT_THROW({
            SetUp("inputs/input_nohouse.txt");
        }, std::runtime_error);
    }

    TEST_P(SimulatorTest, RobotNoDockingStation)
    {
        EXPECT_THROW({
            SetUp("inputs/input_nodock.txt");
        }, std::runtime_error);
    }

    TEST_P(SimulatorTest, RobotFilledLine)
    {
        SetUp("inputs/input_filledline.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, statistics.mission_status);
        EXPECT_EQ(0, statistics.dirt_left);
        EXPECT_TRUE(statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotFilledCol)
    {
        SetUp("inputs/input_filledcol.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, statistics.mission_status);
        EXPECT_EQ(0, statistics.dirt_left);
        EXPECT_TRUE(statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotDeterministic)
    {
        auto algo_factory = GetParam();
        std::unique_ptr<AbstractAlgorithm> first_algorithm = algo_factory();
        Simulator first_simulator;
        first_simulator.readHouseFile("inputs/input_sanity.txt");
        first_simulator.setAlgorithm(*first_algorithm);
        first_simulator.run();

        SimulationStatistics& first_statistics = first_simulator.getSimulationStatistics();

        std::vector<Step> first_runtime_steps(first_statistics.step_history);

        std::unique_ptr<AbstractAlgorithm> second_algorithm = algo_factory();
        Simulator second_simulator;
        second_simulator.readHouseFile("inputs/input_sanity.txt");
        second_simulator.setAlgorithm(*second_algorithm);
        second_simulator.run();

        SimulationStatistics& second_statistics = second_simulator.getSimulationStatistics();

        std::vector<Step> second_runtime_steps(second_statistics.step_history);

        EXPECT_EQ(first_runtime_steps.size(), second_runtime_steps.size());

        for (std::size_t i = 0; i< first_runtime_steps.size(); i++)
        {
            EXPECT_EQ(first_runtime_steps.at(i), second_runtime_steps.at(i));
        }

        EXPECT_TRUE(second_statistics.is_at_docking_station);
    }

    TEST_P(SimulatorTest, RobotImmediateFinish)
    {
        const std::size_t total_dirt = 45;
        const std::size_t dirt_factor = 300;
        SetUp("inputs/input_immediatefinish.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, statistics.mission_status);
        EXPECT_EQ(total_dirt, statistics.dirt_left);
        EXPECT_TRUE(statistics.is_at_docking_station);
        EXPECT_EQ(0, statistics.num_steps_taken);
        EXPECT_EQ(Step::Finish, statistics.step_history.front());
        /* Only dirt should affect score */
        EXPECT_EQ(total_dirt * dirt_factor, statistics.score);
    }

    TEST_P(SimulatorTest, RobotStepsTaken)
    {
        SetUp("inputs/input_stepstaken.txt");

        SimulationStatistics& statistics = getSimulationStatistics();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, statistics.mission_status);
        EXPECT_EQ(0, statistics.dirt_left);
        EXPECT_TRUE(statistics.is_at_docking_station);
        /* Only steps should affect score */
        EXPECT_EQ(statistics.num_steps_taken, statistics.score);
    }

    // Instantiate the test suite with the object pointers
    INSTANTIATE_TEST_SUITE_P(
        SimulatorTests,                     // Name of the test suite
        SimulatorTest,                      // Name of the test fixture
        testing::ValuesIn(algo_factories)   // Parameters to pass to the tests
    );

    TEST(SimulatorAPI, RobotAPICallingOrder)
    {
        Simulator simulator;
        std::unique_ptr<AbstractAlgorithm> algorithm = algo_factories.at(0)();

        EXPECT_THROW({
            simulator.setAlgorithm(algorithm);
        }, std::logic_error);

        EXPECT_THROW({
            simulator.run();
        }, std::logic_error);

        simulator.readHouseFile("inputs/input_sanity.txt");

        EXPECT_THROW({
            simulator.run();
        }, std::logic_error);

        simulator.setAlgorithm(algorithm);

        EXPECT_THROW({
            simulator.readHouseFile("inputs/input_sanity.txt");
        }, std::logic_error);

        simulator.run();
    }

    TEST(MockAlgorithm, RobotIsDead)
    {
        const std::size_t dead_penalty = 2000;

        Simulator simulator;
        MockAlgorithm mock_algorithm;

        simulator.readHouseFile("inputs/input_mockalgo_dead.txt");
        simulator.setAlgorithm(mock_algorithm);

        ON_CALL(mock_algorithm, nextStep())
            .WillByDefault(testing::Return(Step::East));

        simulator.run();

        SimulationStatistics& statistics = simulator.getSimulationStatistics();

        // Check that isDead condition applies here:
        EXPECT_NE(Step::Finish, statistics.step_history.back());
        EXPECT_FALSE(statistics.is_at_docking_station);
        // Invariant: And we know that battery is exhausted, since MaxBattery = 1

        // Dead penalty should be applied
        EXPECT_EQ(dead_penalty + mock_algorithm.getMaxSteps(), statistics.score);
    }

    TEST(MockAlgorithm, RobotIsWorkingNotInDock)
    {
        const std::size_t non_docking_penalty = 1000;
        const std::size_t dirt_factor = 300;

        Simulator simulator;
        MockAlgorithm mock_algorithm;

        simulator.readHouseFile("inputs/input_mockalgo_working.txt");
        simulator.setAlgorithm(mock_algorithm);

        ON_CALL(mock_algorithm, nextStep())
            .WillByDefault(testing::Return(Step::South));

        simulator.run();

        SimulationStatistics& statistics = simulator.getSimulationStatistics();

        // Check that isWorking condition applies here:
        bool is_dead = Step::Finish != statistics.step_history.back() && !statistics.is_at_docking_station && false; // && battery is exhausted (which is false)
        bool is_lying = Step::Finish == statistics.step_history.back() && !statistics.is_at_docking_station;

        EXPECT_FALSE(is_dead || is_lying);

        EXPECT_EQ(dirt_factor * statistics.dirt_left 
                  + non_docking_penalty
                  + statistics.num_steps_taken, statistics.score);
    }

    TEST(MockAlgorithm, RobotIsWorkingInDock)
    {
        const std::size_t dirt_factor = 300;

        Simulator simulator;
        MockAlgorithm mock_algorithm;

        simulator.readHouseFile("inputs/input_mockalgo_working.txt");
        simulator.setAlgorithm(mock_algorithm);

        ON_CALL(mock_algorithm, nextStep())
            .WillByDefault(testing::Return(Step::Finish));

        simulator.run();

        SimulationStatistics& statistics = simulator.getSimulationStatistics();

        // Check that isWorking condition applies here:
        bool is_dead = Step::Finish != statistics.step_history.back() && !statistics.is_at_docking_station && false; // && battery is exhausted (which is false)
        bool is_lying = Step::Finish == statistics.step_history.back() && !statistics.is_at_docking_station;

        EXPECT_FALSE(is_dead || is_lying);

        EXPECT_EQ(dirt_factor * statistics.dirt_left 
                  + statistics.num_steps_taken, statistics.score);
    }

    /* TODO: this test currently doesn't pass due to an alleged contradiction in guidelines.
       There's an open issue about it: https://moodle.tau.ac.il/mod/forum/discuss.php?d=96436
       
       DONE: https://moodle.tau.ac.il/mod/forum/discuss.php?d=109220 */
    TEST(MockAlgorithm, RobotIsLying)
    {
        const std::size_t lying_penalty = 3000;

        Simulator simulator;
        MockAlgorithm mock_algorithm;


        simulator.readHouseFile("inputs/input_stepstaken.txt");
        simulator.setAlgorithm(mock_algorithm);

        EXPECT_CALL(mock_algorithm, nextStep())
            .WillOnce(testing::Return(Step::East))
            .WillOnce(testing::Return(Step::Finish));

        simulator.run();

        SimulationStatistics& statistics = simulator.getSimulationStatistics();

        // Check that isLying condition applies here:
        EXPECT_FALSE(statistics.is_at_docking_station);
        EXPECT_EQ(Step::Finish, statistics.step_history.back());

        EXPECT_EQ(lying_penalty
                  + mock_algorithm.getMaxSteps(), statistics.score);
    }
}
