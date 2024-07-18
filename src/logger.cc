#include "logger.h"

Logger::~Logger()
{
    deleteAllLogFiles();
}

void Logger::logMessage(LogLevel log_level, LogOutput output, const std::string& message)
{
    const std::string& log_prefix = log_level_prefix.at(log_level);

    if (LogOutput::File == output)
    {
        for (auto& file : log_files)
        {
            file << log_prefix << message << std::endl;
        }
    }

    else if (LogOutput::Console == output)
    {
        std::cout << log_prefix << message << std::endl;
    }
}
