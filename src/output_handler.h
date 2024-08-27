#ifndef OUTPUT_HANDLER_H_
#define OUTPUT_HANDLER_H_

#include <map>
#include <vector>
#include <cstddef>
#include <sstream>
#include <iostream>
#include <filesystem>

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
    static constexpr const char kStatisticsSeparator = '-';

    inline static constexpr const char kStepsNumField[] = "NumSteps = ";
    inline static constexpr const char kDirtLeftField[] = "\nDirtLeft = ";
    inline static constexpr const char kStatusField[] = "\nStatus = ";
    inline static constexpr const char kStepsField[] = "\nSteps:\n";
    inline static constexpr const char kInDockField[] = "\nInDock = ";
    inline static constexpr const char kScoreField[] = "\nScore = ";

    static std::string getErrorFileName(const std::string& module_name) { return module_name + kErrorExtension; }

    static std::string getStatisticsFileName(const std::string& algorithm_name, const std::string& house_name)
    {
        return house_name + kStatisticsSeparator + algorithm_name + kStatisticsExtension;
    }

    static void exportToFile(const std::string& file_name, const std::string& message);

    static bool isError(const std::string& error_string) { return !error_string.empty(); }

public:
    OutputHandler() = delete;

    static void printError(const std::string& error_message) { std::cerr << error_message << std::endl; }

    static void printMessage(const std::string& message) { std::cout << message << std::endl; }

    static void exportError(const std::string& module_name, const std::string& error_message)
    {
        if (isError(error_message))
        {
            exportToFile(getErrorFileName(module_name), error_message);
            printError(error_message);
        }
    }

    static void exportStatistics(const std::string& algorithm_name,
                                 const std::string& house_name,
                                 const SimulationStatistics& statistics);

    static void exportSummary(const std::map<std::string, std::map<std::string, std::size_t>>& scores);
};

#endif /* OUTPUT_HANDLER_H_ */
