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
    string_stream << kStepsNumField << statistics.total_steps_taken \
                    << kDirtLeftField << statistics.dirt_left \
                    << kStatusField << statistics.mission_status \
                    << kInDockField << (statistics.is_at_docking_station ? "TRUE" : "FALSE") \
                    << kScoreField << statistics.score \
                    << kStepsField << statistics.steps_taken.str();

    exportToFile(getStatisticsFileName(algorithm_name, house_name), string_stream.str());
}

void OutputHandler::exportSummary(const std::map<std::string, std::map<std::string, std::size_t>>& scores)
{
    std::ostringstream summary;

    // Insert header
    summary << "Algorithm";

    for (const auto& algorithm_map : scores)
    {
        for (const auto& house_score : algorithm_map.second)
        {
            // Insert house name
            summary << "," << house_score.first;
        }
    }
    summary << std::endl;

    // Insert scores
    for (const auto& algorithm_map : scores)
    {
        // Insert algorithm name
        summary << algorithm_map.first;

        for (const auto& house_score : algorithm_map.second)
        {
            summary << "," << house_score.second;
        }
        summary << std::endl;
    }

    exportToFile("summary.csv", summary.str());
}
