#ifndef ROBOT_LOGGER_H_
#define ROBOT_LOGGER_H_

#include <vector>
#include <cstddef>
#include <sstream>
#include <filesystem>

#include "enum_operators.h"
#include "position.h"
#include "logger.h"
#include "status.h"
#include "enums.h"

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
    inline static constexpr const char kStepsNumField[] = "NumSteps = ";
    inline static constexpr const char kDirtLeftField[] = "\nDirtLeft = ";
    inline static constexpr const char kStatusField[] = "\nStatus = ";
    inline static constexpr const char kStepsField[] = "\nSteps:\n";
    inline static constexpr const char kInDockField[] = "\nInDock = ";
    inline static constexpr const char kScoreField[] = "\nScore = ";

    inline static std::ostringstream steps_taken;

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

    std::string getFileName(const std::string& file_path) const
    {
        std::string file_name = std::filesystem::path(file_path).filename().string();
        return file_name;
    }

    /**
     * @brief Add a log file based on the input house file path.
     * @param house_file_path The path of the input house file.
     */
    void initializeLogFile(const std::string& house_file_path)
    {
        std::string house_file_name = getFileName(house_file_path);
        const std::string log_file_name = kOutputFilePrefix + house_file_name;
        addLogFile(log_file_name);
    }

    /**
     * @brief Log a robot step.
     * @param step_moved The step the robot moved.
     * @param current_position The current position of the robot.
     */
    void logRobotStep(Step step_moved)
    {
        steps_taken << step_moved;
    }

    /**
     * @brief Log cleaning statistics.
     * @param total_steps The total number of steps taken by the robot.
     * @param total_dirt The total amount of dirt left.
     * @param is_battery_exhausted Whether the vacuum cleaner's battery is exhausted.
     * @param is_mission_complete Whether the cleaning mission is complete.
     * @param is_at_docking_station Whether the robot is at the docking station.
     * @param score The score of the cleaning mission.
     */
    void logCleaningStatistics(std::size_t total_steps, std::size_t total_dirt, Status status, bool is_at_docking_station, std::size_t score)
    {
        std::ostringstream stringStream;
        stringStream << kStepsNumField << total_steps \
                     << kDirtLeftField << total_dirt \
                     << kStatusField << status \
                     << kInDockField << (is_at_docking_station ? "TRUE" : "FALSE") \
                     << kScoreField << score \
                     << kStepsField << steps_taken.str();

        logMessage(LogLevel::Info, LogOutput::File, stringStream.str());

        steps_taken.str("");
    }

    /**
     * @brief Log a warning message.
     * @param warning_message The warning message to log.
     */
    void logWarning(const std::string& warning_message)
    {
        logMessage(LogLevel::Warning, LogOutput::Console, warning_message);
    }

    /**
     * @brief Log an error message.
     * @param error_message The error message to log.
     */
    void logError(const std::string& error_message)
    {
        logMessage(LogLevel::Error, LogOutput::Console, error_message);
    }
};

#endif /* ROBOT_LOGGER_H_ */
