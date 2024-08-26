#include "logger.h"

void Logger::logMessage(LogLevel log_level, LogOutput log_type, const std::string& message, const std::string& file_path = "")
{
    const std::string& log_prefix = log_level_prefix.at(log_level);
    std::ofstream output_file;

    output_file.open(file_path);
    if (!output_file.is_open())
    {
        throw std::runtime_error("Logger couldn't open output file \"" << file_path << "\"");
    }

    if (LogOutput::File == log_type)
    {
        output_file << log_prefix << message << std::endl;
    }

    else if (LogOutput::Console == log_type || LogOutput::Combined == log_type)
    {
        std::cout << log_prefix << message << std::endl;
    }

    output_file.close();
}
