#ifndef VACUUM_LOGGER_H_
#define VACUUM_LOGGER_H_

#include <exception>
#include <iostream>
#include <ofstream>
#include <map>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR
}

enum class LogOutput
{
    FILE,
    CONSOLE
}

class Logger:
    static std::ofstream log_file;
    static const std::map<LogLevel, const std::string> log_level_prefix;

public:
    Logger(const std::string& log_file_name)
    {
        log_file.open(log_file_name);
        if (!log_file.is_open())
        {
            throw std::system_error("Couldn't open log file");
        }
    }
    static void logMessage(LogLevel log_level, LogOutput output, const std::string& message);

#endif /* VACUUM_LOGGER_H_ */