#include "logger.h"

Logger::~Logger()
{
    deleteAllLogFiles();
}

void Logger::logMessage(LogLevel log_level, LogOutput output, const std::string& message)
{
    const std::string& log_prefix = log_level_prefix.at(log_level);
    std::vector<std::ofstream>& files = (LogLevel::Error == log_level) ? error_files : log_files;

    if (LogOutput::File == output)
    {
        for (auto& file : files)
        {
            file << log_prefix << message << std::endl;
        }
    }

    else if (LogOutput::Console == output)
    {
        std::cout << log_prefix << message << std::endl;
    }
}
