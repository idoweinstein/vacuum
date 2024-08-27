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
    inline static constexpr const char kStepsNumField[] = "NumSteps = ";
    inline static constexpr const char kDirtLeftField[] = "\nDirtLeft = ";
    inline static constexpr const char kStatusField[] = "\nStatus = ";
    inline static constexpr const char kStepsField[] = "\nSteps:\n";
    inline static constexpr const char kInDockField[] = "\nInDock = ";
    inline static constexpr const char kScoreField[] = "\nScore = ";

    static std::string getErrorFileName(const std::string& module_name) { return module_name + ".error"; }

    static std::string getStatisticsFileName(const std::string& algorithm_name,
                                             const std::string& house_name)
    { return house_name + "-" + algorithm_name + ".txt"; }

    static void exportToFile(const std::string& file_name, const std::string& message);

    static bool isError(const std::ostringstream& error_buffer) { return !error_buffer.view().empty(); }

public:
    OutputHandler() = delete;

    static void printError(const std::string& error_message) { std::cerr << error_message << std::endl; }

    static void printMessage(const std::string& message) { std::cout << message << std::endl; }

    static void exportError(const std::string& module_name,
                            const std::string& error_message)
    { exportToFile(getErrorFileName(module_name), error_message); }

    static void exportStatistics(const std::string& algorithm_name,
                                 const std::string& house_name,
                                 const SimulationStatistics& statistics);

    static void exportSummary(const std::map<std::string, std::map<std::string, std::size_t>>& scores);
};

#endif /* OUTPUT_HANDLER_H_ */
