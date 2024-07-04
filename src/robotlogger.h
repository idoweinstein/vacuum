#ifndef VACUUM_ROBOTLOGGER_H_
#define VACUUM_ROBOTLOGGER_H_

#include <format>

#include "direction.h"
#include "position.h"
#include "logger.h"

// A Non-Inheritable Singleton Class Inherits From Singleton Logger
class RobotLogger: public Logger
{
    // Constant RobotLogger strings
    inline static constexpr const char kRobotFinishPrompt[] = "[FINISH] Robot finished cleaning all accessible places!";
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
    static RobotLogger& getInstance()
    {
        static RobotLogger robot_logger_instance; // Instantiated on first getInstance() call only
        return robot_logger_instance;
    }

    void addLogFileFromInput(const std::string& input_file_name)
    {
        const std::string log_file_name = kOutputFilePrefix + input_file_name;
        addLogFile(log_file_name);
    }

    void logRobotFinish()
    {
        logMessage(LogLevel::INFO, LogOutput::FILE, kRobotFinishPrompt);
    }

    void logRobotStep(Direction direction_moved, Position current_position)
    {
        const std::string step_log_message = std::format(kStepFormat,
                                                         direction_moved,
                                                         current_position.first,
                                                         current_position.second);

        logMessage(LogLevel::INFO, LogOutput::FILE, step_log_message);
    }

    void logCleaningStatistics(unsigned int total_steps, unsigned int total_dirt, bool is_battery_exhausted, bool is_mission_complete)
    {
        const std::string statistics_log_message = std::format(kStatisticsFormat,
                                                               total_steps,
                                                               total_dirt,
                                                               is_battery_exhausted,
                                                               is_mission_complete);

        logMessage(LogLevel::INFO, LogOutput::FILE, statistics_log_message);
    }

    void logWarning(const std::string warning_message)
    {
        logMessage(LogLevel::WARNING, LogOutput::CONSOLE, warning_message);
    }

    void logError(const std::string error_message)
    {
        logMessage(LogLevel::ERROR, LogOutput::CONSOLE, error_message);
    }
};

#endif /* VACUUM_ROBOTLOGGER_H_ */
