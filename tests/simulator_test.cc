#include "gtest/gtest.h"

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <ios>

#include "direction.h"
#include "simulator.h"
#include "algorithm.h"
#include "robotlogger.h"
#include "deserializer.h"

namespace
{
    struct RobotState
    {
        std::vector<Step> runtime_steps;
        unsigned int total_steps_taken;
        unsigned int total_dirt_left;
        Status status;
    };

    class OutputDeserializer
    {
        inline static const std::map<char, Step> step_map = {
            {'N', Step::NORTH},
            {'E', Step::EAST},
            {'S', Step::SOUTH},
            {'W', Step::WEST},
            {'s', Step::STAY},
            {'F', Step::FINISH}
        };

        inline static const std::map<std::string, Status> status_map = {
            {"FINISHED", Status::FINISHED},
            {"WORKING", Status::WORKING},
            {"DEAD", Status::DEAD}
        };

        inline static const std::string kRegexPattern = 
            "NumSteps\\s+=\\s+([0-9]+)\r?\n"
            "DirtLeft\\s+=\\s+([0-9]+)\r?\n"
            "Status\\s+=\\s+(FINISHED|WORKING|DEAD)\r?\n"
            "Steps:\r?\n"
            "([NESWs]+F?)";

        std::ifstream output_file;

        bool safeFileOpen(const std::string& output_file_name);
        std::string getNextValue();
        void deserializeStatistics();

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
 
    public:
        RobotState robot_state;

        OutputDeserializer()
        {
            robot_state.total_steps_taken = 0;
            robot_state.total_dirt_left = 0;
            robot_state.status = Status::WORKING;
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
        robot_state.runtime_steps.clear();
        robot_state.runtime_steps.reserve(matches[4].length());
        std::cout << matches[1] << " " << matches[2] << " " << matches[3] << " " << matches[4] << std::endl;
        for (auto step : static_cast<const std::string &>(matches[4]))
        {
            std::cout << step;
            robot_state.runtime_steps.push_back(charToStep(step));
        }
        std::cout << std::endl << "Finished" << std::endl;

        EXPECT_EQ(robot_state.total_steps_taken, robot_state.runtime_steps.size() - 1);

        return true;
    }

    class SimulatorTest : public testing::Test
    {
        OutputDeserializer deserializer;
    protected:

        void SetUp(const std::string& input_file, const std::string& output_file)
        {
            RobotLogger& logger = RobotLogger::getInstance();

            logger.initializeLogFile(input_file);

            Simulator simulator;
            Algorithm algorithm;

            simulator.readHouseFile(input_file);
            simulator.setAlgorithm(algorithm);
            simulator.run();

            EXPECT_TRUE(deserializer.deserializeOutputFile(output_file));
        }

        void TearDown() override
        {
            RobotLogger& logger = RobotLogger::getInstance();

            logger.deleteAllLogFiles();
        }

        RobotState& getRobotState()
        {
            return deserializer.robot_state;
        }
    };

    TEST_F(SimulatorTest, RobotSanity)
    {
        SetUp("inputs/input_sanity.txt", "output_input_sanity.txt");

        RobotState& robot_state = getRobotState();

        // Make sure robot did 'total_steps_taken' steps, and its first step wasn't Direction::STAY
        std::size_t path_length = robot_state.runtime_steps.size();
        if (robot_state.runtime_steps.back() == Step::FINISH) {
            path_length--;
        }
        EXPECT_EQ(robot_state.total_steps_taken, path_length);
        EXPECT_NE(Step::STAY, robot_state.runtime_steps.at(0));

        // Assert the expected program results (Robot is not dead and cleaned all dirt)
        EXPECT_EQ(Status::FINISHED, robot_state.status);
    }

    TEST_F(SimulatorTest, RobotTrappedDirt)
    {
        SetUp("inputs/input_trappeddirt.txt", "output_input_trappeddirt.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected program results (Robot is not dead, cleaned all ACCESSIBLE dirt, but there's more trapped dirt)
        EXPECT_EQ(Status::FINISHED, robot_state.status);
        EXPECT_LT(0, robot_state.total_dirt_left);
    }

    TEST_F(SimulatorTest, RobotMaze)
    {
        SetUp("inputs/input_maze.txt", "output_input_maze.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::FINISHED, robot_state.status);
    }

    TEST_F(SimulatorTest, RobotMinimalBatteryToComplete)
    {
        SetUp("inputs/input_minbattery.txt", "output_input_minbattery.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::FINISHED, robot_state.status);

        Step expected_steps[] = {
            Step::EAST,
            Step::STAY,
            Step::WEST,
            Step::FINISH
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
    }

    TEST_F(SimulatorTest, RobotTooDistantDirt)
    {
        SetUp("inputs/input_distantdirt.txt", "output_input_distantdirt.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::WORKING, robot_state.status);

        unsigned int max_robot_steps = 100;
        EXPECT_EQ(max_robot_steps, robot_state.total_steps_taken);

        unsigned int total_dirt_count = 1;
        EXPECT_EQ(total_dirt_count, robot_state.total_dirt_left);
    }

    TEST_F(SimulatorTest, RobotAllCharacters)
    {
        SetUp("inputs/input_allchars.txt", "output_input_allchars.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::FINISHED, robot_state.status);
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

        RobotState& robot_state = getRobotState();

        // Assert the expected results (should add docking station to end of first row)
        EXPECT_EQ(Status::FINISHED, robot_state.status);
    }

    TEST_F(SimulatorTest, RobotFilledLine)
    {
        SetUp("inputs/input_filledline.txt", "output_input_filledline.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::FINISHED, robot_state.status);
        EXPECT_EQ(0, robot_state.total_dirt_left);
    }

    TEST_F(SimulatorTest, RobotFilledCol)
    {
        SetUp("inputs/input_filledcol.txt", "output_input_filledcol.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_EQ(Status::FINISHED, robot_state.status);
        EXPECT_EQ(0, robot_state.total_dirt_left);
    }
}
