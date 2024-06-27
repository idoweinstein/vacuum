#include "logger.h"

void Logger::logMessage(LogLevel log_level, LogOutput output, const std::string& message)
{
    const std::string log_prefix = log_level_prefix.at(log_level);

    if (LogOutput::FILE == output)
    {
        log_file << log_prefix << message << std::endl;
    }
    else
    {
        std::cout << log_prefix << message << std::endl;
    }
}