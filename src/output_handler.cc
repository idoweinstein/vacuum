#include "output_handler.h"

#include <fstream>
#include <sstream>
#include <string>

void OutputHandler::exportToFile(const std::string& file_name, const std::string& message)
{
    std::ofstream output_file;

    output_file.open(file_name, std::ios_base::app);
    if (!output_file.is_open())
    {
        throw std::runtime_error("Couldn't open output file \"" + file_name + "\"");
    }

    output_file << message << std::endl;

    output_file.close();
}

void OutputHandler::exportStatistics(const std::string& algorithm_name,
                                     const std::string& house_name,
                                     const SimulationStatistics& statistics)
{
    std::ostringstream string_stream;
    string_stream << kStepsNumField << statistics.num_steps_taken \
                    << kDirtLeftField << statistics.dirt_left \
                    << kStatusField << statistics.mission_status \
                    << kInDockField << (statistics.is_at_docking_station ? "TRUE" : "FALSE") \
                    << kScoreField << statistics.score \
                    << kStepsField;

    for (const auto& step : statistics.step_history)
    {
        string_stream << step;
    }

    exportToFile(getStatisticsFileName(algorithm_name, house_name), string_stream.str());
}

void OutputHandler::exportSummary(const std::map<std::string, std::map<std::string, std::size_t>>& scores)
{
    std::ostringstream summary;

    // Insert header
    summary << "Algo \\ House";

    if (!scores.empty())
    {
        auto first_row = scores.begin();
        for (const auto& house_column : first_row->second)
        {
            // Insert house name
            summary << "," << house_column.first;
        }
        summary << std::endl;
    }

    // Insert scores
    for (const auto& algorithm_row : scores)
    {
        // Insert algorithm name
        summary << algorithm_row.first;

        for (const auto& house_column : algorithm_row.second)
        {
            summary << "," << house_column.second;
        }
        summary << std::endl;
    }

    exportToFile("summary.csv", summary.str());
}
