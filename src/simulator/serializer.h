#ifndef SERIALIZER_H_
#define SERIALIZER_H_

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
 * @class Serializer
 * @brief TODO
 */
class Serializer
{
    struct SerializationType
    {
        STATISTICS,
        ERROR
    };

    // Constant Serializer strings
    inline static constexpr const char kStepsNumField[] = "NumSteps = ";
    inline static constexpr const char kDirtLeftField[] = "\nDirtLeft = ";
    inline static constexpr const char kStatusField[] = "\nStatus = ";
    inline static constexpr const char kStepsField[] = "\nSteps:\n";
    inline static constexpr const char kInDockField[] = "\nInDock = ";
    inline static constexpr const char kScoreField[] = "\nScore = ";

public:
    Serializer() = delete;

    /**
     * @brief Get the file name from a given file path.
     * @return The extracted file name.
     */
    std::string getFileName(const std::string& file_path) const
    {
        std::string file_name = std::filesystem::path(file_path).filename().string();
        return file_name;
    }

    static std::string constructErrorFileName(const std::string& module_name)
    {
        return module_name + ".error";
    }

    static std::string constructOutputFileName(const std::string& algorithm_name, const std::string& house_name)
    {
        return house_name + "-" + algorithm_name + ".txt";
    }

    static void serializeToFile(SerializationType type, const std::string& file_name, const std::ostringstream& string_stream)
    {
        std::ofstream output_file;

        output_file.open(file_name, std::ios_base::app);
        if (!output_file.is_open())
        {
            throw std::runtime_error("Serializer couldn't open output file \"" << file_name << "\"");
        }

        output_file << string_stream << std::endl;
    }

    static bool isError(const std::ostringstream& error_buffer)
    {
        return !error_buffer.view().empty();
    }

    static void serializeErrors(const std::string& algorithm_name,
                                const std::string& house_name,
                                const std::ostringstream& algorithm_error_buffer,
                                const std::ostringstream& house_error_buffer)
    {
        if (isError(algorithm_error_buffer))
        {
            serializeToFile(SerializationType::ERROR, constructErrorFileName(algorithm_name), algorithm_error_buffer);
        }

        if (isError(house_error_buffer))
        {
            serializeToFile(SerializationType::ERROR, constructErrorFileName(house_name), house_error_buffer);
        }
    }

    static void serializeStatistics(const std::string& algorithm_name,
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

        serializeToFile(SerializationType::STATISTICS, constructOutputFileName(algorithm_name, house_name), string_stream);
    }
};

#endif /* SERIALIZER_H_ */
