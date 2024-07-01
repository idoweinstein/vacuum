#ifndef VACUUM_LOGGER_H_
#define VACUUM_LOGGER_H_

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

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

// An Inheritable Singleton Class
class Logger
{
    std::vector<std::ofstream> log_files;

    inline static const std::map<LogLevel, const std::string> log_level_prefix = {
        {LogLevel::INFO, ""},
        {LogLevel::WARNING, "[WARNING] "},
        {LogLevel::ERROR, "[ERROR] "}
    };

protected:
    /* Protected ctor & dtor - So it WON'T be used externally, but WILL be inherited */
    Logger() {}
    ~Logger();

public:
    static Logger& getInstance()
    {
        static Logger logger;
        return logger;
    }
    /* To preserve Singleton property - Delete copy constructor & assignment operator */
    Logger(const Logger& logger) = delete;
    Logger& operator=(const Logger& logger) = delete;

    void addLogFile(const std::string& log_file_name)
    {
        std::ofstream& new_file = log_files.emplace_back();
        new_file.open(log_file_name);
        if (!new_file.is_open())
        {
            throw std::runtime_error("Couldn't open log file");
        }
    }

    void logMessage(LogLevel log_level, LogOutput output, const std::string& message);
};

#endif /* VACUUM_LOGGER_H_ */