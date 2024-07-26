#include <string>
#include <cstdlib>
#include <exception>

#include "robot_logger.h"
#include "algorithm.h"
#include "simulator.h"

namespace Constants
{
    constexpr int kNumberOfArguments = 2;
    constexpr int kHouseFileArgument = 1;
}

int main(int argc, char* argv[])
{
    RobotLogger& logger = RobotLogger::getInstance();

    if (Constants::kNumberOfArguments == argc)
    {
        const std::string input_file_path(argv[Constants::kHouseFileArgument]);

        try
        {
            Simulator simulator;
            simulator.readHouseFile(input_file_path);
            Algorithm algorithm;
            simulator.setAlgorithm(algorithm);
            simulator.run();
        }
        catch(const std::exception& exception)
        {
            logger.logError(exception.what());
            return EXIT_FAILURE;
        }
    }

    else
    {
        logger.logError("Invalid number of arguments!\nUsage: myrobot <input_file>");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
