#ifndef VACUUM_ROBOTDESERIALIZER_H_
#define VACUUM_ROBOTDESERIALIZER_H_

#include <map>
#include <vector>
#include <string>
#include <istream>
 
#include "position.h"
#include "robot.h"

/**
 * @brief The RobotDeserializer class is responsible for deserializing robot data from a file.
 *
 * It provides methods to deserialize parameters and the house layout from an input stream.
 * The deserialized data is used to create a Robot object.
 */
class RobotDeserializer
{
    enum BlockType : char
    {
        DOCKING_STATION = '@',
        WALL = 'X',
        CLEAN = ' ',
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

    enum Parameter
    {
        END_OF_PARAMETER = -1,
        MAX_BATTERY_STEPS = 0,
        MAX_ROBOT_STEPS,
        NUMBER_OF_PARAMETERS
    };

    static constexpr const char kParameterDelimiter = ' ';

    static const std::map<std::string, Parameter> parameter_map;

    /**
     * @brief Converts a string value to an unsigned integer.
     *
     * @param value The string value to convert.
     * @return The converted unsigned integer value.
     */
    static unsigned int valueToUnsignedInt(const std::string& value);

    /**
     * @brief Stores a parameter value in the parameters array.
     *
     * @param parameters The array to store the parameter value.
     * @param key The parameter key.
     * @param value The parameter value.
     * @return True if the parameter was stored successfully, false otherwise.
     */
    static bool storeParameter(unsigned int* parameters, const std::string& key, const std::string& value);

    /**
     * @brief Deserializes the parameters from an input stream.
     *
     * In case a required parameter is missing, a default value of '0' will be used instead.
     *
     * @param parameters The array to store the deserialized parameters.
     * @param input_stream The input stream to read the parameters from.
     */
    static void deserializeParameters(unsigned int* parameters, std::istream& input_stream);

    /**
     * @brief Deserializes the house layout from an input stream.
     *
     * @param wall_map The 2D vector to store the wall layout.
     * @param dirt_map The 2D vector to store the dirt level layout.
     * @param docking_station_position The position of the docking station.
     * @param input_stream The input stream to read the house layout from.
     */
    static void deserializeHouse(std::vector<std::vector<bool>>& wall_map,
                                 std::vector<std::vector<unsigned int>>& dirt_map,
                                 Position& docking_station_position,
                                 std::istream& input_stream);

public:
    /**
     * @brief Deserializes robot data from a file.
     *
     * @param input_file_name The name of the input file.
     * @return The deserialized Robot object.
     */
    static Robot deserializeFromFile(const std::string& input_file_name);
};

#endif /* VACUUM_DESERIALIZER_H_ */
