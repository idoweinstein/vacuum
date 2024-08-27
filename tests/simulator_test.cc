#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <ios>
#include <regex>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include "common/enums.h"

#include "simulator/deserializer.h"
#include "simulator/simulator.h"

#include "algorithm/a/algorithm.h" // TODO: change

using namespace std::string_literals;

namespace
{
    struct RobotState
    {
        std::vector<Step> runtime_steps;
        std::size_t total_steps_taken;
        std::size_t total_dirt_left;
        Status status;
        bool in_dock;
        std::size_t score;
    };

    class OutputDeserializer
    {
        inline static const std::map<char, Step> step_map = {
            {'N', Step::North},
            {'E', Step::East},
            {'S', Step::South},
            {'W', Step::West},
            {'s', Step::Stay},
            {'F', Step::Finish}
        };

        inline static const std::map<std::string, Status> status_map = {
            {"FINISHED", Status::Finished},
            {"WORKING", Status::Working},
            {"DEAD", Status::Dead}
        };

        inline static const std::string kRegexPattern = 
            "NumSteps\\s+=\\s+([0-9]+)\r?\n"
            "DirtLeft\\s+=\\s+([0-9]+)\r?\n"
            "Status\\s+=\\s+(FINISHED|WORKING|DEAD)\r?\n"
            "InDock\\s+=\\s+(TRUE|FALSE)\r?\n"
            "Score\\s+=\\s+([0-9]+)\r?\n"
            "Steps:\r?\n"
            "([NESWs]*F?)";

        std::ifstream output_file;

        bool safeFileOpen(const std::string& output_file_name);
        std::string getNextValue();

        static Step charToStep(char step_string)
        {
            EXPECT_TRUE(step_map.contains(step_string));
            return step_map.at(step_string);
        }

        static Status stringToStatus(const std::string& status_string)
        {
            EXPECT_TRUE(status_map.contains(status_string));
            return status_map.at(status_string);
        }

        static bool stringToBool(const std::string& bool_string)
        {
            return bool_string == "TRUE"s;
        }
 
    public:
        RobotState robot_state;

        OutputDeserializer()
        {
            robot_state.total_steps_taken = 0;
            robot_state.total_dirt_left = 0;
            robot_state.status = Status::Working;
        }

        bool deserializeOutputFile(const std::string& output_file_name);
    };

    bool OutputDeserializer::safeFileOpen(const std::string& output_file_name)
    {
        output_file.open(output_file_name);
        bool is_opened = output_file.is_open();

        EXPECT_TRUE(is_opened);
        return is_opened;
    }

    bool OutputDeserializer::deserializeOutputFile(const std::string& output_file_name)
    {
        bool is_opened = safeFileOpen(output_file_name);
        if (!is_opened)
        {
            return false;
        }

        std::stringstream buffer;
        buffer << output_file.rdbuf();

        std::regex pattern(kRegexPattern);

        std::smatch matches;
        std::string content = buffer.str();

        if (!std::regex_search(content, matches, pattern))
        {
            return false;
        }

        robot_state = {};
        robot_state.total_steps_taken = std::stoi(matches[1]);
        robot_state.total_dirt_left = std::stoi(matches[2]);
        robot_state.status = stringToStatus(matches[3]);
        robot_state.in_dock = stringToBool(matches[4]);
        robot_state.score = std::stoi(matches[5]);
        robot_state.runtime_steps.clear();
        robot_state.runtime_steps.reserve(matches[6].length());
        for (auto step : static_cast<const std::string &>(matches[6]))
        {
            robot_state.runtime_steps.push_back(charToStep(step));
        }

        EXPECT_GE(robot_state.total_steps_taken, robot_state.runtime_steps.size() - 1);
        EXPECT_LE(robot_state.total_steps_taken, robot_state.runtime_steps.size());

        return true;
    }

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

    class SimulatorTest : public testing::Test
    {
        OutputDeserializer deserializer;
    protected:

        void SetUp(const std::string& input_file, const std::string& output_file)
        {
            Simulator simulator;
            Algorithm algorithm;

            simulator.readHouseFile(input_file);
            simulator.setAlgorithm(algorithm);
            simulator.run();

            EXPECT_TRUE(deserializer.deserializeOutputFile(output_file));
        }

        RobotState& getRobotState()
        {
            return deserializer.robot_state;
        }
    };

    TEST_F(SimulatorTest, RobotSanity)
    {
        SetUp("inputs/input_sanity.txt", "output_input_sanity.txt");

        RobotState robot_state = getRobotState();

        // Make sure robot did 'total_steps_taken' steps, and its first step wasn't Direction::STAY
        std::size_t path_length = robot_state.runtime_steps.size();
        if (robot_state.runtime_steps.back() == Step::Finish) {
            path_length--;
        }
        EXPECT_EQ(robot_state.total_steps_taken, path_length);
        EXPECT_NE(Step::Stay, robot_state.runtime_steps.at(0));

        // Assert the expected program results (Robot is not dead and cleaned all dirt)
        EXPECT_EQ(Status::Finished, robot_state.status);

        EXPECT_TRUE(robot_state.in_dock);
    }

    TEST_F(SimulatorTest, RobotTrappedDirt)
    {
        SetUp("inputs/input_trappeddirt.txt", "output_input_trappeddirt.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected program results (Robot is not dead, cleaned all ACCESSIBLE dirt, but there's more trapped dirt)
        EXPECT_EQ(Status::Finished, robot_state.status);
        EXPECT_LT(0, robot_state.total_dirt_left);

        EXPECT_TRUE(robot_state.in_dock);
    }

    TEST_F(SimulatorTest, RobotMaze)
    {
        SetUp("inputs/input_maze.txt", "output_input_maze.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, robot_state.status);

        EXPECT_TRUE(robot_state.in_dock);
    }

    TEST_F(SimulatorTest, RobotMinimalBatteryToComplete)
    {
        SetUp("inputs/input_minbattery.txt", "output_input_minbattery.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, robot_state.status);

        Step expected_steps[] = {
            Step::East,
            Step::Stay,
            Step::West,
            Step::Finish
        };
        size_t expected_steps_num = 4;

        if (expected_steps_num != robot_state.runtime_steps.size())
        {
            FAIL();
            return;
        }

        for (unsigned int i = 0; i < expected_steps_num; i++)
        {
            EXPECT_EQ(expected_steps[i], robot_state.runtime_steps.at(i));
        }

        EXPECT_TRUE(robot_state.in_dock);
    }

    TEST_F(SimulatorTest, RobotTooDistantDirt)
    {
        SetUp("inputs/input_distantdirt.txt", "output_input_distantdirt.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, robot_state.status);

        EXPECT_GE(50, robot_state.total_steps_taken);

        EXPECT_EQ(1, robot_state.total_dirt_left);

        EXPECT_TRUE(robot_state.in_dock);
    }

    TEST_F(SimulatorTest, RobotAllCharacters)
    {
        SetUp("inputs/input_allchars.txt", "output_input_allchars.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, robot_state.status);

        EXPECT_TRUE(robot_state.in_dock);
    }

    TEST_F(SimulatorTest, RobotNoHouse)
    {
        EXPECT_THROW({
            SetUp("inputs/input_nohouse.txt", "output_input_nohouse.txt");
        }, std::runtime_error);
    }

    TEST_F(SimulatorTest, RobotNoDockingStation)
    {
        EXPECT_THROW({
            SetUp("inputs/input_nodock.txt", "output_input_nodock.txt");
        }, std::runtime_error);
    }

    TEST_F(SimulatorTest, RobotFilledLine)
    {
        SetUp("inputs/input_filledline.txt", "output_input_filledline.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, robot_state.status);
        EXPECT_EQ(0, robot_state.total_dirt_left);
        EXPECT_TRUE(robot_state.in_dock);
    }

    TEST_F(SimulatorTest, RobotFilledCol)
    {
        SetUp("inputs/input_filledcol.txt", "output_input_filledcol.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, robot_state.status);
        EXPECT_EQ(0, robot_state.total_dirt_left);
        EXPECT_TRUE(robot_state.in_dock);
    }

    TEST_F(SimulatorTest, RobotDeterministic)
    {
        SetUp("inputs/input_sanity.txt", "output_input_sanity.txt");

        std::vector<Step> first_runtime_steps = getRobotState().runtime_steps;

        SetUp("inputs/input_sanity.txt", "output_input_sanity.txt");

        std::vector<Step> second_runtime_steps = getRobotState().runtime_steps;

        EXPECT_EQ(first_runtime_steps.size(), second_runtime_steps.size());

        for (std::size_t i = 0; i< first_runtime_steps.size(); i++)
        {
            EXPECT_EQ(first_runtime_steps.at(i), second_runtime_steps.at(i));
        }

        EXPECT_TRUE(getRobotState().in_dock);
    }

    TEST_F(SimulatorTest, RobotImmediateFinish)
    {
        const std::size_t total_dirt = 45;
        const std::size_t dirt_factor = 300;
        SetUp("inputs/input_immediatefinish.txt", "output_input_immediatefinish.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, robot_state.status);
        EXPECT_EQ(total_dirt, robot_state.total_dirt_left);
        EXPECT_TRUE(robot_state.in_dock);
        EXPECT_EQ(0, robot_state.total_steps_taken);
        EXPECT_EQ(Step::Finish, robot_state.runtime_steps.front());
        /* Only dirt should affect score */
        EXPECT_EQ(total_dirt * dirt_factor, robot_state.score);
    }

    TEST_F(SimulatorTest, RobotStepsTaken)
    {
        SetUp("inputs/input_stepstaken.txt", "output_input_stepstaken.txt");

        RobotState robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::Finished, robot_state.status);
        EXPECT_EQ(0, robot_state.total_dirt_left);
        EXPECT_TRUE(robot_state.in_dock);
        /* Only steps should affect score */
        EXPECT_EQ(robot_state.total_steps_taken, robot_state.score);
    }

    TEST(SimulatorAPI, RobotAPICallingOrder)
    {
        Simulator simulator;
        Algorithm algorithm;

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

        OutputDeserializer deserializer;
        Simulator simulator;
        MockAlgorithm mock_algorithm;

        simulator.readHouseFile("inputs/input_mockalgo_dead.txt");
        simulator.setAlgorithm(mock_algorithm);

        ON_CALL(mock_algorithm, nextStep())
            .WillByDefault(testing::Return(Step::East));

        simulator.run();
        EXPECT_TRUE(deserializer.deserializeOutputFile("output_input_mockalgo_dead.txt"));

        EXPECT_EQ(Status::Dead, deserializer.robot_state.status);

        EXPECT_FALSE(deserializer.robot_state.in_dock);
        // Dead penalty should be applied
        EXPECT_EQ(dead_penalty + mock_algorithm.getMaxSteps(), deserializer.robot_state.score);
    }

    TEST(MockAlgorithm, RobotIsWorking)
    {
        const std::size_t non_docking_penalty = 1000;
        const std::size_t dirt_factor = 300;

        OutputDeserializer deserializer;
        Simulator simulator;
        MockAlgorithm mock_algorithm;

        simulator.readHouseFile("inputs/input_mockalgo_working.txt");
        simulator.setAlgorithm(mock_algorithm);

        ON_CALL(mock_algorithm, nextStep())
            .WillByDefault(testing::Return(Step::South));

        simulator.run();
        EXPECT_TRUE(deserializer.deserializeOutputFile("output_input_mockalgo_working.txt"));

        EXPECT_FALSE(deserializer.robot_state.in_dock);

        EXPECT_EQ(Status::Working, deserializer.robot_state.status);
        EXPECT_EQ(dirt_factor * deserializer.robot_state.total_dirt_left 
                  + non_docking_penalty
                  + deserializer.robot_state.total_steps_taken, deserializer.robot_state.score);
    }

    /* TODO: this test currently doesn't pass due to an alleged contradiction in guidelines.
       There's an open issue about it: https://moodle.tau.ac.il/mod/forum/discuss.php?d=96436 */
    TEST(MockAlgorithm, RobotIsLying)
    {
        const std::size_t lying_penalty = 3000;

        OutputDeserializer deserializer;
        Simulator simulator;
        MockAlgorithm mock_algorithm;


        simulator.readHouseFile("inputs/input_stepstaken.txt");
        simulator.setAlgorithm(mock_algorithm);

        EXPECT_CALL(mock_algorithm, nextStep())
            .WillOnce(testing::Return(Step::East))
            .WillOnce(testing::Return(Step::Finish));

        simulator.run();
        EXPECT_TRUE(deserializer.deserializeOutputFile("output_input_stepstaken.txt"));

        EXPECT_FALSE(deserializer.robot_state.in_dock);

        EXPECT_EQ(Status::Finished, deserializer.robot_state.status);
        EXPECT_EQ(lying_penalty
                  + mock_algorithm.getMaxSteps(), deserializer.robot_state.score);
    }
}
