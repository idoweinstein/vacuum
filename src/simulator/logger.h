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
    Warning,
    Error
};

/**
 * @brief Enumeration representing different log outputs.
 */
enum class LogOutput
{
    File,
    Console
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
        {LogLevel::Warning, "[WARNING] "}, // Prefix for warning level logs
        {LogLevel::Error, "[ERROR] "}      // Prefix for error level logs
    };

    std::vector<std::ofstream> log_files;  // Vector of log file streams
    std::vector<std::ofstream> error_files;  // Vector of error file streams

protected:
    Logger() {}
    virtual ~Logger();

public:
    /**
     * @brief Get the instance of the Logger class.
     *
     * This method returns the instance of the Logger class.
     * It ensures that only one instance of the logger exists throughout the program.
     *
     * @return The instance of the Logger class.
     */
    static Logger& getInstance()
    {
        static Logger logger;
        return logger;
    }

    /**
     * @brief Deleted copy constructor and assignment operator.
     *
     * The copy constructor and assignment operator are deleted to enforce the singleton property of the Logger class.
     */
    Logger(const Logger& logger) = delete;
    Logger& operator=(const Logger& logger) = delete;

    /**
     * @brief Add a log file to the logger.
     *
     * This method adds a log file to the logger.
     * The log file will be opened and used for logging messages.
     *
     * @param log_file_name The name of the log file to add.
     * @throws std::runtime_error if the log file cannot be opened.
     */
    virtual void addLogFile(const std::string& log_file_name)
    {
        std::ofstream& new_file = log_files.emplace_back();
        new_file.open(log_file_name);

        if (!new_file.is_open())
        {
            throw std::runtime_error("Couldn't open log file");
        }
    }

    /**
     * @brief Add an error file to the logger.
     *
     * This method adds an error file to the logger.
     * The error file will be opened and used for logging error messages.
     *
     * @param error_file_name The name of the error file to add.
     * @throws std::runtime_error if the error file cannot be opened.
     */
    virtual void addErrorFile(const std::string& error_file_name)
    {
        std::ofstream& new_file = error_files.emplace_back();
        new_file.open(error_file_name);

        if (!new_file.is_open())
        {
            throw std::runtime_error("Couldn't open error file");
        }
    }

    /**
     * @brief Deletes all previously added log files.
     * 
     * This method deletes all previously added log files, using `addLogFile()` calls.
     */
    virtual void deleteAllLogFiles()
    {
        for (int i = log_files.size() - 1; i >= 0; i--)
        {
            log_files.at(i).close();
            log_files.pop_back();
        }
    }

    /**
     * @brief Deletes all previously added error files.
     * 
     * This method deletes all previously added error files, using `addErrorFile()` calls.
     */
    virtual void deleteAllErrorFiles()
    {
        for (int i = error_files.size() - 1; i >= 0; i--)
        {
            error_files.at(i).close();
            error_files.pop_back();
        }
    }

    /**
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
    virtual void logMessage(LogLevel log_level, LogOutput output, const std::string& message);
};

#endif /* LOGGER_H_ */
