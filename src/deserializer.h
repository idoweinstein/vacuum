#ifndef VACUUM_DESERIALIZER_H_
#define VACUUM_DESERIALIZER_H_

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <istream>
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
    static constexpr const int kDefaultParameterValue = 0;

    static constexpr const bool kDefaultIsWall = false;
    static constexpr const unsigned int kDefaultDirtLevel = 0;

    static const std::string kMaxStepsParameter = "MaxSteps";
    static const std::string kMaxBatteryParameter = "MaxBattery";
    static const std::string kHouseRowsNumParameter = "Rows";
    static const std::string kHouseColsNumParameter = "Cols";

    struct Parameter
    {
        bool is_initialized = false;
        unsigned int value = kDefaultParameterValue;
    };

    struct DockingStation
    {
        bool is_initialized = false;
        Position position;
    }

    enum BlockType : char
    {
        DOCKING_STATION = 'D',
        WALL = 'W',
        DIRT_LEVEL_0 = '0',
        DIRT_LEVEL_1 = '1',
        DIRT_LEVEL_2 = '2',
        DIRT_LEVEL_3 = '3',
        DIRT_LEVEL_4 = '4',
        DIRT_LEVEL_5 = '5',
        DIRT_LEVEL_6 = '6',
        DIRT_LEVEL_7 = '7',
        DIRT_LEVEL_8 = '8',
        DIRT_LEVEL_9 = '9'
    };

    enum ParameterType
    {
        MAX_BATTERY_STEPS = 0,
        MAX_SIMULATOR_STEPS,
        HOUSE_ROWS_NUM,
        HOUSE_COLS_NUM,
        NUMBER_OF_PARAMETERS
    };

    static void assertParameterSet(Parameter& parameter, const char* parameter_name)
    {
        if (!parameter.is_initialized)
        {
            const std::string error_message = "Missing " + parameter_name + " parameter in house file!";
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
     * @return true if house grid was given in input stream, false otherwise.
     */
    static bool deserializeParameter(std::istream& input_stream, const std::string& parameter_name);

public:
    static unsigned int deserializeMaxSteps(std::istream& input_stream);

    static std::unique_ptr<Battery> SimulatorDeserializer::deserializeBattery(std::istream& input_stream);

    /**
     * @brief Deserializes the house layout from an input stream.
     *
     * @param wall_map The 2D vector to store the wall layout.
     * @param dirt_map The 2D vector to store the dirt level layout.
     * @param docking_station_position The position of the docking station.
     * @param input_stream The input stream to read the house layout from.
     */
    static void deserializeHouse(std::istream& input_stream);

#endif /* VACUUM_DESERIALIZER_H_ */
