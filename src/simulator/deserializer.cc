#include "deserializer.h"

#include <string>
#include <vector>
#include <memory>
#include <ranges>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <optional>
#include <stdexcept>

#include "common/position.h"

#include "battery.h"
#include "house.h"

/* 
 * Trim helper function.
 * Removes leading and trailing whitespaces from a string.
 * Source: https://stackoverflow.com/questions/66897068/can-trim-of-a-string-be-done-inplace-with-c20-ranges#answer-66897681
 */
void Deserializer::trimSpaces(std::string& input_string)
{
    auto is_not_space = [](unsigned char input_char){ return !std::isspace(input_char); };

    // Erase the the spaces at the back first - so we don't have to do extra work
    input_string.erase(
        std::ranges::find_if(input_string | std::views::reverse, is_not_space).base(),
        input_string.end()
    );

    // Erase the spaces at the front
    input_string.erase(
        input_string.begin(),
        std::ranges::find_if(input_string, is_not_space)
    );
}

std::size_t Deserializer::valueToUnsignedNumber(const std::string& value)
{
    std::istringstream value_stream(value);
    int numerical_value = 0;

    value_stream >> numerical_value;
    if (!value_stream)
    {
        throw std::runtime_error("A parameter with non-integer value was given!");
    }

    if (numerical_value < 0)
    {
        throw std::runtime_error("A parameter with negative value was given!");
    }

    return (std::size_t)numerical_value;
}

std::size_t Deserializer::deserializeParameter(std::istream& input_stream, const std::string& parameter_name)
{
    std::optional<std::size_t> parameter;

    std::string line;
    std::getline(input_stream, line);
    std::istringstream line_stream(line);

    std::string parameter_key;
    if (std::getline(line_stream, parameter_key, kParameterDelimiter))
    {
        trimSpaces(parameter_key);

        std::string parameter_value;
        std::getline(line_stream, parameter_value);

        if (parameter_name == parameter_key)
        {
            trimSpaces(parameter_value);
            parameter = valueToUnsignedNumber(parameter_value);
        }
    }

    assertParameterSet(parameter, parameter_name);

    return parameter.value();
}

void Deserializer::ignoreInternalName(std::istream& input_stream)
{
    // Unused input line - used for internal naming
    std::string house_internal_name;
    std::getline(input_stream, house_internal_name);
}

std::size_t Deserializer::deserializeMaxSteps(std::istream& input_stream)
{
    std::size_t max_simulator_steps = deserializeParameter(input_stream, kMaxStepsParameter);

    return max_simulator_steps;
}

std::unique_ptr<Battery> Deserializer::deserializeBattery(std::istream& input_stream)
{
    std::size_t full_battery_capacity = deserializeParameter(input_stream, kMaxBatteryParameter);

    return std::make_unique<Battery>(full_battery_capacity);
}

std::unique_ptr<House> Deserializer::deserializeHouse(std::istream& input_stream)
{
    std::optional<Position> docking_station_position;

    std::size_t house_rows_num = deserializeParameter(input_stream, kHouseRowsNumParameter);
    std::size_t house_cols_num = deserializeParameter(input_stream, kHouseColsNumParameter);

    // Initialize Dirt & Wall maps with their default values
    auto wall_map = std::make_unique<std::vector<std::vector<bool>>>(
        std::vector<std::vector<bool>>(
            house_rows_num,
            std::vector<bool> (house_cols_num, kDefaultIsWall)
        )
    );

    auto dirt_map = std::make_unique<std::vector<std::vector<unsigned int>>>(
        std::vector<std::vector<unsigned int>>(
            house_rows_num,
            std::vector<unsigned int> (house_cols_num, kDefaultDirtLevel)
        )
    );

    std::string house_block_row;
    std::size_t row_index = 0;

    while (std::getline(input_stream, house_block_row))
    {
        if (row_index >= house_rows_num)
        {
            break;
        }

        std::size_t column_index = 0;

        for (char block : house_block_row)
        {
            if (column_index >= house_cols_num)
            {
                break;
            }

            switch (block)
            {
                case BlockType::DirtLevel0:
                case BlockType::DirtLevel1:
                case BlockType::DirtLevel2:
                case BlockType::DirtLevel3:
                case BlockType::DirtLevel4:
                case BlockType::DirtLevel5:
                case BlockType::DirtLevel6:
                case BlockType::DirtLevel7:
                case BlockType::DirtLevel8:
                case BlockType::DirtLevel9:
                    (*dirt_map)[row_index][column_index] = static_cast<unsigned int>(block - '0');
                    break;

                case BlockType::DockingStation:
                    if (docking_station_position.has_value())
                    {
                        throw std::runtime_error("More than one docking station was given in house file!");
                    }
                    docking_station_position = {static_cast<int>(row_index), static_cast<int>(column_index)};
                    break;

                case BlockType::Wall:
                    (*wall_map)[row_index][column_index] = true;
                    break;

                default:
                    break; // Space as well as any invalid characters means Clear Block (dirt level of '0')
            }
            column_index++;
        }
        row_index++;
    }

    if (!docking_station_position.has_value())
    {
        throw std::runtime_error("Missing docking station position in house file!");
    }

    return std::make_unique<House>(std::move(wall_map), std::move(dirt_map), docking_station_position.value());
}
