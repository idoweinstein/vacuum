#include "gtest/gtest.h"

#include <filesystem>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <ios>

#include "robot.h"
#include "direction.h"
#include "robotlogger.h"
#include "robotdeserializer.h"

namespace fs = std::filesystem;

namespace
{
    struct RobotState
    {
        std::vector<Direction> runtime_steps;
        bool is_robot_finished;
        bool is_battery_exhausted;
        bool is_mission_succeeded;
        unsigned int total_steps_taken;
        unsigned int total_dirt_left;
    };

    class OutputDeserializer
    {
        inline static constexpr const char kStatisticsDelimiter = ':';
        inline static const std::string kStepPrefix = "[STEP]";
        inline static const std::string kFinishPrefix = "[FINISH]";
        inline static const std::string kProgramTerminationPrefix = "###";
        inline static const std::map<std::string, Direction> direction_map = {
            {"North", Direction::NORTH},
            {"East", Direction::EAST},
            {"South", Direction::SOUTH},
            {"West", Direction::WEST},
            {"Stay", Direction::STAY}
        };

        std::ifstream output_file;

        bool safeFileOpen(const std::string& output_file_name);
        std::string getNextValue();
        void deserializeStatistics();

        static Direction stringToDirection(const std::string& direction_string)
        {
            EXPECT_TRUE(direction_map.contains(direction_string));
            return direction_map.at(direction_string);
        }
 
    public:
        RobotState robot_state;

        OutputDeserializer()
        {
            robot_state.is_robot_finished = false;
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

    std::string OutputDeserializer::getNextValue()
    {
        std::string key;
        std::string value;
        std::string line;

        std::getline(output_file, line);
        std::istringstream line_stream(line);

        std::getline(line_stream, value, kStatisticsDelimiter);
        std::getline(line_stream, key);
        return key;
    }

    void OutputDeserializer::deserializeStatistics()
    {
        std::istringstream total_steps_taken(getNextValue());
        total_steps_taken >> robot_state.total_steps_taken;

        std::istringstream total_dirt_left(getNextValue());
        total_dirt_left >> robot_state.total_dirt_left;

        std::istringstream is_battery_exhausted(getNextValue());
        is_battery_exhausted >> std::boolalpha >> robot_state.is_battery_exhausted;

        std::istringstream is_mission_succeeded(getNextValue());
        is_mission_succeeded >> std::boolalpha >> robot_state.is_mission_succeeded;
    }

    bool OutputDeserializer::deserializeOutputFile(const std::string& output_file_name)
    {
        bool is_opened = safeFileOpen(output_file_name);
        if (!is_opened)
        {
            return false;
        }

        std::string line;
        while (std::getline(output_file, line))
        {
            std::istringstream line_stream(line);

            std::string prefix;
            line_stream >> prefix;

            std::string direction;
            if (kStepPrefix == prefix)
            {
                for (int i = 0; i < 5; i++)
                {
                    line_stream >> direction;
                }

                robot_state.runtime_steps.push_back(stringToDirection(direction));
            }

            else if (kFinishPrefix == prefix)
            {
                robot_state.is_robot_finished = true;
            }

            else
            {
                deserializeStatistics();
            }
        }

        return true;
    }

    class RobotTest : public testing::Test
    {
        OutputDeserializer deserializer;
    protected:

        void SetUp(const std::string& input_file, const std::string& output_file)
        {
            RobotLogger& logger = RobotLogger::getInstance();

            Robot robot = RobotDeserializer::deserializeFromFile(input_file);
            std::string input_file_name = fs::path(input_file).filename().string();
            logger.addLogFileFromInput(input_file_name);

            robot.run();
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

    TEST_F(RobotTest, RobotSanity)
    {
        SetUp("inputs/input_sanity.txt", "output_input_sanity.txt");

        RobotState& robot_state = getRobotState();

        // Make sure robot did 'total_steps_taken' steps, and its first step wasn't Direction::STAY
        EXPECT_EQ(robot_state.total_steps_taken, robot_state.runtime_steps.size());
        EXPECT_NE(Direction::STAY, robot_state.runtime_steps.at(0));

        // Assert the expected program results (Robot is not dead and cleaned all dirt)
        EXPECT_TRUE(robot_state.is_robot_finished);
        EXPECT_FALSE(robot_state.is_battery_exhausted);
        EXPECT_TRUE(robot_state.is_mission_succeeded);
    }

    TEST_F(RobotTest, RobotTrappedDirt)
    {
        SetUp("inputs/input_trappeddirt.txt", "output_input_trappeddirt.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected program results (Robot is not dead, cleaned all ACCESSIBLE dirt, but there's more trapped dirt)
        EXPECT_TRUE(robot_state.is_robot_finished);
        EXPECT_FALSE(robot_state.is_battery_exhausted);
        EXPECT_FALSE(robot_state.is_mission_succeeded);
    }

    TEST_F(RobotTest, RobotMaze)
    {
        SetUp("inputs/input_maze.txt", "output_input_maze.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_TRUE(robot_state.is_robot_finished);
        EXPECT_FALSE(robot_state.is_battery_exhausted);
        EXPECT_TRUE(robot_state.is_mission_succeeded);
    }

    TEST_F(RobotTest, RobotMinimalBatteryToComplete)
    {
        SetUp("inputs/input_minbattery.txt", "output_input_minbattery.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_TRUE(robot_state.is_robot_finished);
        EXPECT_TRUE(robot_state.is_battery_exhausted);
        EXPECT_TRUE(robot_state.is_mission_succeeded);

        Direction expected_steps[] = {
            Direction::EAST,
            Direction::STAY,
            Direction::WEST,
        };
        size_t expected_steps_num = 3;

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

    TEST_F(RobotTest, RobotNoDirt)
    {
        SetUp("inputs/input_nodirt.txt", "output_input_nodirt.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_TRUE(robot_state.is_robot_finished);
        EXPECT_FALSE(robot_state.is_battery_exhausted);
        EXPECT_TRUE(robot_state.is_mission_succeeded);

        EXPECT_EQ(0, robot_state.total_steps_taken);
    }

    TEST_F(RobotTest, RobotTooDistantDirt)
    {
        SetUp("inputs/input_distantdirt.txt", "output_input_distantdirt.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_FALSE(robot_state.is_robot_finished);
        EXPECT_TRUE(robot_state.is_battery_exhausted);
        EXPECT_FALSE(robot_state.is_mission_succeeded);

        unsigned int max_robot_steps = 100;
        EXPECT_EQ(max_robot_steps, robot_state.total_steps_taken);

        unsigned int total_dirt_count = 1;
        EXPECT_EQ(total_dirt_count, robot_state.total_dirt_left);
    }

    TEST_F(RobotTest, RobotAllCharacters)
    {
        SetUp("inputs/input_allchars.txt", "output_input_allchars.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results
        EXPECT_TRUE(robot_state.is_robot_finished);
        EXPECT_FALSE(robot_state.is_battery_exhausted);
        EXPECT_TRUE(robot_state.is_mission_succeeded);

        EXPECT_EQ(69, robot_state.total_steps_taken);
    }

    TEST_F(RobotTest, RobotNoHouse)
    {
        SetUp("inputs/input_nohouse.txt", "output_input_nohouse.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results (should be a 1x1 house with docking station only)
        EXPECT_TRUE(robot_state.is_robot_finished);
        EXPECT_FALSE(robot_state.is_battery_exhausted);
        EXPECT_TRUE(robot_state.is_mission_succeeded);

        EXPECT_EQ(0, robot_state.total_steps_taken);
    }

    TEST_F(RobotTest, RobotNoDockingStation)
    {
        SetUp("inputs/input_nodock.txt", "output_input_nodock.txt");

        RobotState& robot_state = getRobotState();

        // Assert the expected results (should add docking station to end of first row)
        EXPECT_TRUE(robot_state.is_robot_finished);
        EXPECT_FALSE(robot_state.is_battery_exhausted);
        EXPECT_TRUE(robot_state.is_mission_succeeded);
    }
}
