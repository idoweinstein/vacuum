#ifndef VACUUM_ROBOTLOGGER_H_
#define VACUUM_ROBOTLOGGER_H_

#include <format>

#include "logger.h"
#include "position.h"
#include "direction.h"

/**
 * @class RobotLogger
 * @brief A Non-Inheritable Singleton Class that Inherits From Singleton Logger.
 *
 * The RobotLogger class is a singleton class that provides logging functionality for a robot.
 * It inherits from the Singleton Logger class and implements methods to log robot steps,
 * cleaning statistics, warnings, and errors. The class also provides a method to add a log file
 * based on an input file name.
 *
 * Note: The RobotLogger class has no default copy constructor and default assignment operator,
 * since, Logger, its base class deleted them.
 */
class RobotLogger: public Logger
{
    // Constant RobotLogger strings
    inline static constexpr const char kOutputFilePrefix[] = "output_";
    inline static constexpr const char kStepFormat[] = "[STEP] Robot took step to {} - New Position ({},{})";
    inline static constexpr const char kStatisticsFormat[] = "### Program Terminated ###\n"
                                                  "Total Steps Taken: {}\n"
                                                  "Total Dirt Left: {}\n"
                                                  "Is Vacuum Cleaner Dead: {}\n"
                                                  "Mission Succeeded: {}";

    /* Private ctor & dtor - So it WON'T be used externally and WON'T be inherited */
    RobotLogger() {}
    ~RobotLogger() {}

public:
    /**
     * @brief Get the instance of the RobotLogger class.
     * @return The instance of the RobotLogger class.
     */
    static RobotLogger& getInstance()
    {
        static RobotLogger robot_logger_instance; // Instantiated on first getInstance() call only
        return robot_logger_instance;
    }

    /**
     * @brief Add a log file based on the input file name.
     * @param input_file_name The name of the input file.
     */
    void addLogFileFromInput(const std::string& input_file_name)
    {
        const std::string& log_file_name = kOutputFilePrefix + input_file_name;
        addLogFile(log_file_name);
    }

    /**
     * @brief Log a robot step.
     * @param direction_moved The direction the robot moved.
     * @param current_position The current position of the robot.
     */
    void logRobotStep(Direction direction_moved, Position current_position)
    {
        const std::string step_log_message = std::format(kStepFormat,
                                                         direction_moved,
                                                         current_position.first,
                                                         current_position.second);

        logMessage(LogLevel::INFO, LogOutput::FILE, step_log_message);
    }

    /**
     * @brief Log cleaning statistics.
     * @param total_steps The total number of steps taken by the robot.
     * @param total_dirt The total amount of dirt left.
     * @param is_battery_exhausted Whether the vacuum cleaner's battery is exhausted.
     * @param is_mission_complete Whether the cleaning mission is complete.
     */
    void logCleaningStatistics(unsigned int total_steps, unsigned int total_dirt, bool is_battery_exhausted, bool is_mission_complete)
    {
        const std::string statistics_log_message = std::format(kStatisticsFormat,
                                                               total_steps,
                                                               total_dirt,
                                                               is_battery_exhausted,
                                                               is_mission_complete);

        logMessage(LogLevel::INFO, LogOutput::FILE, statistics_log_message);
    }

    /**
     * @brief Log a warning message.
     * @param warning_message The warning message to log.
     */
    void logWarning(const std::string warning_message)
    {
        logMessage(LogLevel::WARNING, LogOutput::CONSOLE, warning_message);
    }

    /**
     * @brief Log an error message.
     * @param error_message The error message to log.
     */
    void logError(const std::string error_message)
    {
        logMessage(LogLevel::ERROR, LogOutput::CONSOLE, error_message);
    }
};

#endif /* VACUUM_ROBOTLOGGER_H_ */
