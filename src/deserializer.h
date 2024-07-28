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
    static constexpr const char kParameterDelimiter = '=';                // The delimiter between a parameter name and its value.

    static constexpr const bool kDefaultIsWall = false;                   // The default value for a wall block.
    static constexpr const unsigned int kDefaultDirtLevel = 0;            // The default value for a dirt block.

    inline static const std::string kMaxStepsParameter = "MaxSteps";      // The parameter name for the maximum number of steps.
    inline static const std::string kMaxBatteryParameter = "MaxBattery";  // The parameter name for the maximum battery capacity.
    inline static const std::string kHouseRowsNumParameter = "Rows";      // The parameter name for the number of rows in the house.
    inline static const std::string kHouseColsNumParameter = "Cols";      // The parameter name for the number of columns in the house.

    /**
     * @brief The BlockType enum represents the different types of blocks in the house layout.
     */
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

    /**
     * @brief Removes leading and trailing spaces from a string.
     */
    static void trimSpaces(std::string& input_string);

    /**
     * @brief Asserts that a parameter is set.
     *
     * If the parameter is not set, a runtime_error exception is thrown.
     *
     * @param parameter The parameter to check.
     * @param parameter_name The name of the parameter.
     * @throws std::runtime_error If given parameter is not set.
     */
    static void assertParameterSet(std::optional<std::size_t>& parameter, const std::string& parameter_name)
    {
        if (!parameter.has_value())
        {
            const std::string error_message = "Missing '" + parameter_name + "' parameter in house file!";
            throw std::runtime_error(error_message);
        }
    }

    /**
     * @brief Converts a string value to an unsigned number (std::size_t).
     *
     * If a non-integer / negative string is given, returns a default value (of '0'), instead.
     *
     * @param value The string value to convert.
     * @throws std::runtime_error if value contains a non-integer or a negative number.
     * @return The converted std::size_t value.
     */
    static std::size_t valueToUnsignedNumber(const std::string& value);

    /**
     * @brief Deserializes the parameters from an input stream.
     *
     * In case a required parameter is missing, a default value of '0' will be used instead.
     *
     * @param input_stream The input stream to read the parameters from.
     * @param parameter_name The key of the parameter to be deserialized.
     * @return The value of the deserialized parameter.
     */
    static std::size_t deserializeParameter(std::istream& input_stream, const std::string& parameter_name);

public:
    /**
    * @brief Deleted deault empty constructor.
    *
    * The default empty constructor is deleted since it's useless, as all the Deserializer member functions are `static`.
    */
    Deserializer() = delete;

    /**
     * @brief Reads the house name from the input stream and ignores it.
     * 
     * @param input_stream The input stream to read the house name from.
     */
    static void ignoreInternalName(std::istream& input_stream);

    /**
     * @brief Deserializes the maximum number of steps from an input stream.
     *
     * @param input_stream The input stream to read the maximum number of steps from.
     * @return The deserialized maximum number of steps.
     */
    static std::size_t deserializeMaxSteps(std::istream& input_stream);

    /**
     * @brief Deserializes the maximum battery capacity from an input stream.
     *
     * @param input_stream The input stream to read the maximum battery capacity from.
     * @return unique_ptr to the deserialized Battery object.
     */
    static std::unique_ptr<Battery> deserializeBattery(std::istream& input_stream);

    /**
     * @brief Deserializes the house layout from an input stream.
     *
     * @param input_stream The input stream to read the house layout from.
     * @throws std::runtime_error If there's more / less than one docking station given.
     * @return unique_ptr to the deserialized House object.
     */
    static std::unique_ptr<House> deserializeHouse(std::istream& input_stream);
};

#endif /* VACUUM_DESERIALIZER_H_ */
