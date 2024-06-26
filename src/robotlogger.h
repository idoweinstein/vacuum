#ifndef VACUUM_ROBOTLOGGER_H_
#define VACUUM_ROBOTLOGGER_H_

#include <format>

#include "logger.h"
#include "position.h"
#include "direction.h"

class RobotLogger: public Logger
{
    static const std::string kRobotStepFormat;
    static const std::string kCleaningStatisticsFormat;

public:
    static void logRobotStep(Direction direction_moved, UPosition current_position)
    {
        const std::string step_log_message = std::format(kRobotStepFormat, direction_moved, current_position.first, current_position.right);
        logMessage(LogLevel::INFO, LogOutput::FILE, step_log_message);
    }

    static void logCleaningStatistics(unsigned int total_steps, unsigned int total_dirt, bool is_battery_exhausted, bool is_mission_success)
    {
        const std::string statistics_log_message = std::format(kCleaningStatisticsFormat,
                                                               total_steps,
                                                               total_dirt,
                                                               is_battery_exhausted,
                                                               is_mission_success);
        logMessage(LogLevel::INFO, LogOutput::FILE, statistics_log_message);
    }

    static void logWarning(const std::string warning_message)
    {
        logMessage(LogLevel::WARNING, LogOutput::CONSOLE, warning_message);
    }

    static void logError(const std::string error_message)
    {
        logMessage(LogLevel::ERROR, LogOutput::CONSOLE, error_message);
    }
}

#endif /* VACUUM_ROBOTLOGGER_H_ */