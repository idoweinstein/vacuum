#include <string>
#include <exception>
#include <filesystem>

#include "robotdeserializer.h"
#include "robotlogger.h"
#include "robot.h"

namespace fs = std::filesystem;

namespace Constants
{
    constexpr int kNumberOfArguments = 2;
    constexpr int kInputFileIndex = 1;
}

int main(int argc, char* argv[])
{
    RobotLogger& logger = RobotLogger::getInstance();

    if (argc == Constants::kNumberOfArguments)
    {
        const std::string input_file_path(argv[Constants::kInputFileIndex]);

        try
        {
            std::string input_file_name = fs::path(input_file_path).filename().string();
            logger.addLogFileFromInput(input_file_name);

            Robot robot = RobotDeserializer::deserializeFromFile(input_file_path);

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
