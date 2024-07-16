#ifndef VACUUM_DESERIALIZER_H_
#define VACUUM_DESERIALIZER_H_

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <istream>
#include <optional>
#include <stdexcept>

#include "simulator.h"
#include "position.h"

/**
 * @brief The Deserializer class is responsible for deserializing simulator data from a file.
 *
 * It provides methods to deserialize parameters and the house layout from an input stream.
 * The deserialized data is used to create a Simulator object.
 */
class Deserializer
{
    static constexpr const char kParameterDelimiter = '=';

    static constexpr const bool kDefaultIsWall = false;
    static constexpr const unsigned int kDefaultDirtLevel = 0;

    inline static const std::string kMaxStepsParameter = "MaxSteps";
    inline static const std::string kMaxBatteryParameter = "MaxBattery";
    inline static const std::string kHouseRowsNumParameter = "Rows";
    inline static const std::string kHouseColsNumParameter = "Cols";

    enum BlockType : char
    {
        DockingStation = 'D',
        Wall = 'W',
        DirtLevel0 = '0',
        DirtLevel1 = '1',
        DirtLevel2 = '2',
        DirtLevel3 = '3',
        DirtLevel4 = '4',
        DirtLevel5 = '5',
        DirtLevel6 = '6',
        DirtLevel7 = '7',
        DirtLevel8 = '8',
        DirtLevel9 = '9'
    };

    static void assertParameterSet(std::optional<unsigned int>& parameter, const std::string& parameter_name)
    {
        if (!parameter.has_value())
        {
            const std::string error_message = "Missing '" + parameter_name + "' parameter in house file!";
            throw std::runtime_error(error_message);
        }
    }

    /**
     * @brief Converts a string value to an unsigned integer.
     *
     * If a non-integer / negative string is given, returns a default value (of '0'), instead.
     *
     * @param value The string value to convert.
     * @return The converted unsigned integer value.
     */
    static unsigned int valueToUnsignedInt(const std::string& value);

    /**
     * @brief Deserializes the parameters from an input stream.
     *
     * In case a required parameter is missing, a default value of '0' will be used instead.
     *
     * @param parameters The array to store the deserialized parameters.
     * @param input_stream The input stream to read the parameters from.
     * @return The value of the deserialized parameter.
     */
    static unsigned int deserializeParameter(std::istream& input_stream, const std::string& parameter_name);

public:
    static void ignoreInternalName(std::istream& input_stream);

    static unsigned int deserializeMaxSteps(std::istream& input_stream);

    static std::unique_ptr<Battery> deserializeBattery(std::istream& input_stream);

    /**
     * @brief Deserializes the house layout from an input stream.
     *
     * @param wall_map The 2D vector to store the wall layout.
     * @param dirt_map The 2D vector to store the dirt level layout.
     * @param docking_station_position The position of the docking station.
     * @param input_stream The input stream to read the house layout from.
     */
    static std::unique_ptr<House> deserializeHouse(std::istream& input_stream);
};

#endif /* VACUUM_DESERIALIZER_H_ */
