#include <exception>
#include <iostream>
#include <vector>

#include "robot.h"
#include "robotlogger.h"
#include "robotdeserializer.h"

int main(int argc, char* argv[])
{
    if (2 == argc)
    {
        std::vector<std::vector<bool>> wall_map;
        std::vector<std::vector<unsigned int>> dirt_map;

        const std::string input_file_name(argv[1]);

        RobotLogger::setLogFile(input_file_name);

        Robot robot = RobotDeserializer::deserializeFromFile(wall_map, dirt_map, input_file_name);

        robot.run();
    }

    else
    {
        RobotLogger::logError("Invalid number of arguments!\nUsage: myrobot <input_file>");
    }
}