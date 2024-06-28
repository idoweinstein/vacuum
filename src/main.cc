#include <exception>
#include <iostream>
#include <vector>

#include "robot.h"
#include "robotlogger.h"
#include "robotdeserializer.h"

int main(int argc, char* argv[])
{
    RobotLogger& logger = RobotLogger::getInstance();
    constexpr int kNumberOfArguments = 2;

    if (argc == kNumberOfArguments)
    {
        std::vector<std::vector<bool>> wall_map;
        std::vector<std::vector<unsigned int>> dirt_map;

        const std::string input_file_name(argv[1]);

        logger.addLogFileFromInput(input_file_name);

        Robot robot = RobotDeserializer::deserializeFromFile(wall_map, dirt_map, input_file_name);

        try
        {
            robot.run();
        }
        catch(const std::exception& exception)
        {
            logger.logError(exception.what());
        }
    }

    else
    {
        logger.logError("Invalid number of arguments!\nUsage: myrobot <input_file>");
    }
}