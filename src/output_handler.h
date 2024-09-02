#ifndef OUTPUT_HANDLER_H_
#define OUTPUT_HANDLER_H_

#include <map>
#include <vector>
#include <cstddef>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <unordered_set>

#include "simulator/enum_operators.h"
#include "simulator/simulator.h"
#include "simulator/status.h"

#include "common/enums.h"
#include "common/position.h"

/**
 * @class OutputHandler
 * @brief TODO
 */
class OutputHandler
{
    // Constant OutputHandler strings
    inline static constexpr const char kStatisticsExtension[] = ".txt";
    inline static constexpr const char kErrorExtension[] = ".error";
    inline static constexpr const char kStatisticsSeparator = '-';

    inline static constexpr const char kStepsNumField[] = "NumSteps = ";
    inline static constexpr const char kDirtLeftField[] = "\nDirtLeft = ";
    inline static constexpr const char kStatusField[] = "\nStatus = ";
    inline static constexpr const char kStepsField[] = "\nSteps:\n";
    inline static constexpr const char kInDockField[] = "\nInDock = ";
    inline static constexpr const char kScoreField[] = "\nScore = ";

    inline static std::unordered_set<std::string> output_files;

    /**
     * @brief Constructs the error file name of a given module (algorithm / house).
     * 
     * @param module_name The name of the module (algorithm / house).
     * 
     * @return The constructed error file name.
     */
    static std::string getErrorFileName(const std::string& module_name) { return module_name + kErrorExtension; }

    /**
     * @brief Constructs the output file name of a given algorithm - house pair.
     * 
     * @param algorithm_name The name of the algorithm in the pair.
     * @param house_name The name of the house in the pair.
     * 
     * @return The constructed output file name.
     */
    static std::string getStatisticsFileName(const std::string& algorithm_name, const std::string& house_name)
    {
        return house_name + kStatisticsSeparator + algorithm_name + kStatisticsExtension;
    }

    /**
     * @brief Exports a given message into a given file (using append).
     * 
     * @param file_name The file to append the message into.
     * @param message The message to be appended.
     */
    static void exportToFile(const std::string& file_name, const std::string& message);

    /**
     * @brief Checks whether there's an error or not (by checking whether error string is emptry or not).
     * 
     * @param error_string The error message to be checked.
     * 
     * @return true if error message is not empty; false otherwise.
     */
    static bool isError(const std::string& error_string) { return !error_string.empty(); }

public:
    /**
    * @brief Deleted deault empty constructor.
    *
    * The default empty constructor is deleted since it's useless, as all the OutputHandler member functions are `static`.
    */
    OutputHandler() = delete;

    /**
     * @brief Prints an error message to `cerr` (standard error).
     * 
     * @param module_name The module name the error is related to.
     * @param error_message The error message to be printed.
     */
    static void printError(const std::string& module_name, const std::string& error_message)
    {
        std::cerr << "[ ERROR at " << module_name << " ] " << error_message << std::endl;
    }

    /**
     * @brief Prints an error message to `cout` (standard output).
     * 
     * @param message The message to be printed.
     */
    static void printMessage(const std::string& message) { std::cout << message << std::endl; }

    /**
     * @brief Exports an error message both to console and to a dedicated `.error` message.
     */
    static void exportError(const std::string& module_name, const std::string& error_message)
    {
        if (isError(error_message))
        {
            exportToFile(getErrorFileName(module_name), error_message);
            printError(module_name, error_message);
        }
    }

    /**
     * @brief Export run statistics of a single task, into an output file of an house - algorithm pair.
     * 
     * @param algorithm_name The task's algorithm name for the output file.
     * @param house_name The task's house name for the output file.
     * @param statistics The task's simulation statistics report, to be written into output file.
     */
    static void exportStatistics(const std::string& algorithm_name,
                                 const std::string& house_name,
                                 const SimulationStatistics& statistics);

    /**
     * @brief Export run summary for all tasks, into a `summary.csv` file, from a given scores data structure.
     * 
     * @param scores The scores data structure to export the summary from.
     */
    static void exportSummary(const std::map<std::string, std::map<std::string, std::size_t>>& scores);
};

#endif /* OUTPUT_HANDLER_H_ */
