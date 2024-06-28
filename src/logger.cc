#include "logger.h"

Logger::~Logger()
{
    for (std::ofstream& file: log_files)
    {
        file.close();
    }
}

void Logger::logMessage(LogLevel log_level, LogOutput output, const std::string& message)
{
    const std::string log_prefix = log_level_prefix.at(log_level);

    if (LogOutput::FILE == output)
    {
        for (std::ofstream& file: log_files)
        {
            file << log_prefix << message << std::endl;
        }
    }
    else if (LogOutput::CONSOLE == output)
    {
        std::cout << log_prefix << message << std::endl;
    }
}