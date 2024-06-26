#ifndef VACUUM_LOGGER_H_
#define VACUUM_LOGGER_H_

#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

enum class LogLevel
{
    INFO = 0,
    WARNING,
    ERROR
};

enum class LogOutput
{
    FILE,
    CONSOLE
};

class Logger
{
    inline static std::ofstream log_file;
    inline static const std::map<LogLevel, const std::string> log_level_prefix = {
        {LogLevel::INFO, ""},
        {LogLevel::WARNING, "[WARN]"},
        {LogLevel::ERROR, "[ERROR]"}
    };

public:
    static void setLogFile(const std::string& log_file_name)
    {
        log_file.open(log_file_name);
        if (!log_file.is_open())
        {
            throw std::runtime_error("Couldn't open log file");
        }
    }

    static void logMessage(LogLevel log_level, LogOutput output, const std::string& message);
};

#endif /* VACUUM_LOGGER_H_ */