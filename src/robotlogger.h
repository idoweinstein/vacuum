#ifndef VACUUM_ROBOTLOGGER_H_
#define VACUUM_ROBOTLOGGER_H_

#include <sstream>

#include "direction.h"
#include "position.h"
#include "logger.h"

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
    inline static constexpr const char kRobotFinishPrompt[] = "[FINISH] Robot finished cleaning all accessible places!";
    inline static constexpr const char kOutputFilePrefix[] = "output_";
    inline static constexpr const char kStepFormat1[] = "[STEP] Robot took step to ";
    inline static constexpr const char kStepFormat2[] = " - New Position (";
    inline static constexpr const char kStepFormat3[] = ",";
    inline static constexpr const char kStepFormat4[] = ")";
    inline static constexpr const char kStatisticsFormat1[] = "### Program Terminated ###";
    inline static constexpr const char kStatisticsFormat2[] = "\nTotal Steps Taken: ";
    inline static constexpr const char kStatisticsFormat3[] = "\nTotal Dirt Left: ";
    inline static constexpr const char kStatisticsFormat4[] = "\nIs Battery Exhausted: ";
    inline static constexpr const char kStatisticsFormat5[] = "\nMission Succeeded: ";

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

    virtual std::string getFileName(const std::string& file_path) const
    {
        std::string file_name = fs::path(input_file_path).filename().string();
        return file_name;
    }

    /**
     * @brief Add a log file based on the input house file path.
     * @param house_file_path The path of the input house file.
     */
    virtual void initializeLogFile(const std::string& house_file_path)
    {
        std::string house_file_name = getFileName(house_file_path);
        const std::string log_file_name = kOutputFilePrefix + house_file_name;
        addLogFile(log_file_name);
    }

    /**
     * @brief Log a robot reaching its finish condition.
     */
    virtual void logRobotFinish()
    {
        logMessage(LogLevel::INFO, LogOutput::FILE, kRobotFinishPrompt);
    }

    /**
     * @brief Log a robot step.
     * @param step_moved The step the robot moved.
     * @param current_position The current position of the robot.
     */
    virtual void logRobotStep(Step step_moved, Position current_position)
    {
        std::ostringstream stringStream;
        stringStream << kStepFormat1 << step_moved << kStepFormat2 \
            << current_position.first << kStepFormat3 << current_position.second << kStepFormat4;

        logMessage(LogLevel::INFO, LogOutput::FILE, stringStream.str());
    }

    /**
     * @brief Log cleaning statistics.
     * @param total_steps The total number of steps taken by the robot.
     * @param total_dirt The total amount of dirt left.
     * @param is_battery_exhausted Whether the vacuum cleaner's battery is exhausted.
     * @param is_mission_complete Whether the cleaning mission is complete.
     */
    virtual void logCleaningStatistics(unsigned int total_steps, unsigned int total_dirt, bool is_battery_exhausted, bool is_mission_complete)
    {
        std::ostringstream stringStream;
        stringStream << kStatisticsFormat1 << kStatisticsFormat2 << total_steps << kStatisticsFormat3 \
            << total_dirt << kStatisticsFormat4 << std::boolalpha << is_battery_exhausted << kStatisticsFormat5 \
            << is_mission_complete;

        logMessage(LogLevel::INFO, LogOutput::FILE, stringStream.str());
    }

    /**
     * @brief Log a warning message.
     * @param warning_message The warning message to log.
     */
    virtual void logWarning(const std::string warning_message)
    {
        logMessage(LogLevel::WARNING, LogOutput::CONSOLE, warning_message);
    }

    /**
     * @brief Log an error message.
     * @param error_message The error message to log.
     */
    virtual void logError(const std::string error_message)
    {
        logMessage(LogLevel::ERROR, LogOutput::CONSOLE, error_message);
    }
};

#endif /* VACUUM_ROBOTLOGGER_H_ */
