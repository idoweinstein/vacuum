#include <string>
#include <exception>

#include "robotlogger.h"
#include "algorithm.h"
#include "simulator.h"

namespace fs = std::filesystem;

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
            Algorithm algorithm;

            simulator.readHouseFile(input_file_path);
            simulator.setAlgorithm(algorithm);
            simulator.run();
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
