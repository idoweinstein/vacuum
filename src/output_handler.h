#ifndef OUTPUT_HANDLER_H_
#define OUTPUT_HANDLER_H_

#include <vector>
#include <cstddef>
#include <sstream>
#include <filesystem>

#include "common/enums.h"
#include "common/position.h"

#include "enum_operators.h"
#include "simulator.h"
#include "status.h"


/**
 * @class OutputHandler
 * @brief TODO
 */
class OutputHandler
{
    struct OutputType
    {
        STATISTICS,
        SUMMARY,
        ERROR
    };

    // Constant OutputHandler strings
    inline static constexpr const char kStepsNumField[] = "NumSteps = ";
    inline static constexpr const char kDirtLeftField[] = "\nDirtLeft = ";
    inline static constexpr const char kStatusField[] = "\nStatus = ";
    inline static constexpr const char kStepsField[] = "\nSteps:\n";
    inline static constexpr const char kInDockField[] = "\nInDock = ";
    inline static constexpr const char kScoreField[] = "\nScore = ";

    static std::string getErrorFileName(const std::string& module_name)
    {
        return module_name + ".error";
    }

    static std::string getStatisticsFileName(const std::string& algorithm_name, const std::string& house_name)
    {
        return house_name + "-" + algorithm_name + ".txt";
    }

    static void exportToFile(OutputType type, const std::string& file_name, const std::ostringstream& string_stream)
    {
        std::ofstream output_file;

        output_file.open(file_name, std::ios_base::app);
        if (!output_file.is_open())
        {
            throw std::runtime_error("OutputHandler couldn't open output file \"" << file_name << "\"");
        }

        output_file << string_stream << std::endl;

        output_file.close();
    }

    static bool isError(const std::ostringstream& error_buffer)
    {
        return !error_buffer.view().empty();
    }

public:
    OutputHandler() = delete;

    static void printError(const std::string& error_message)
    {
        std::cerr << error_message << std::endl;
    }

    static void printMessage(const std::string& message)
    {
        std::cout << message << std::endl;
    }

    static void exportError(const std::string& module_name,
                            const std::string& error_message)
    {
        exportToFile(OutputType::ERROR, getErrorFileName(module_name), error_message);
    }

    static void exportErrors(const std::string& algorithm_name,
                             const std::string& house_name,
                             const std::ostringstream& algorithm_error_buffer,
                             const std::ostringstream& house_error_buffer)
    {
        if (isError(algorithm_error_buffer))
        {
            exportToFile(OutputType::ERROR, getErrorFileName(algorithm_name), algorithm_error_buffer);
        }

        if (isError(house_error_buffer))
        {
            exportToFile(OutputType::ERROR, getErrorFileName(house_name), house_error_buffer);
        }
    }

    static void exportStatistics(const std::string& algorithm_name,
                                 const std::string& house_name,
                                 const SimulationStatistics& statistics)
    {
        std::ostringstream string_stream;
        string_stream << kStepsNumField << statistics.total_steps_taken \
                      << kDirtLeftField << statistics.dirt_left \
                      << kStatusField << statistics.mission_status \
                      << kInDockField << (statistics.is_at_docking_station ? "TRUE" : "FALSE") \
                      << kScoreField << statistics.scorescore \
                      << kStepsField << statistics.steps_taken.str();

        exportToFile(OutputType::STATISTICS, getStatisticsFileName(algorithm_name, house_name), string_stream);
    }

    static void exportSummary(const std::map<std::string, std::map<std::string, std::size_t>>& scores)
    {
        std::ostringstream summary;

        // Insert header
        summary << "algorithm";
        for (const auto& algorithm_map : scores)
        {
            for (const auto& house_map : alogrithm_map.second)
            {   
                // Insert house name
                summary << "," << house_map.first;
            }
        }
        summary << std::endl;

        // Insert scores
        for (const auto& algorithm_map : scores)
        {
            // Insert algorithm name
            summary << algorithm_map.first;

            for (const auto& house_map : algorithm_map.second)
            {
                summary << "," << house_map.second;
            }
            summary << std::endl;
        }

        exportToFile(OutputType::SUMMARY, "summary.csv", summary);
    }
};

#endif /* OUTPUT_HANDLER_H_ */
