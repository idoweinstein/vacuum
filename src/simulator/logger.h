#ifndef LOGGER_H_
#define LOGGER_H_

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

/**
 * @brief Enumeration representing different log levels.
 */
enum class LogLevel
{
    Info = 0,
    Error
};

/**
 * @brief Enumeration representing different log outputs.
 */
enum class LogOutput
{
    File,
    Console,
    Combined
};

/**
 * @brief Singleton class for logging messages.
 *
 * The Logger class provides functionality to log messages with different log levels and outputs.
 * It is designed as a singleton class to ensure only one instance of the logger exists.
 */
class Logger
{
    inline static const std::map<LogLevel, const std::string> log_level_prefix = {
        {LogLevel::Info, ""},              // Prefix for information level logs
        {LogLevel::Error, "[ERROR] "}      // Prefix for error level logs
    };

public:
    Logger() = delete;

    /**
     * @brief Deleted copy constructor and assignment operator.
     *
     * The copy constructor and assignment operator are deleted to enforce the singleton property of the Logger class.
     */
    Logger(const Logger& logger) = delete;
    Logger& operator=(const Logger& logger) = delete;

    /**
     * TODO: Fix
     * @brief Log a message with the specified log level and output.
     *
     * This method logs a message with the specified log level and output.
     * The log level determines the prefix of the log message.
     * The output determines where the log message is written (file or console).
     * In case the log message is written to file, it will be written to all files previously added.
     *
     * @param log_level The log level of the message.
     * @param output The log output (file or console).
     * @param message The message to log.
     */
    static void logMessage(LogLevel log_level, LogOutput output, const std::string& message, const std::string& file_path);
};

#endif /* LOGGER_H_ */
