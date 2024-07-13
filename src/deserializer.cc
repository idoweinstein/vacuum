#include "deserializer.h"

#include <string>
#include <vector>
#include <memory>
#include <ranges>
#include <fstream>
#include <sstream>
#include <optional>
#include <stdexcept>

#include "robotlogger.h"
#include "position.h"
#include "battery.h"
#include "house.h"

/* 
 * Trim helper function.
 * Removes leading and trailing whitespaces from a string.
 * Source: https://stackoverflow.com/questions/66897068/can-trim-of-a-string-be-done-inplace-with-c20-ranges#answer-66897681
 */
void trim(std::string& s) {
    auto not_space = [](unsigned char c){ return !std::isspace(c); };

    // erase the the spaces at the back first
    // so we don't have to do extra work
    s.erase(
        std::ranges::find_if(s | std::views::reverse, not_space).base(),
        s.end());

    // erase the spaces at the front
    s.erase(
        s.begin(),
        std::ranges::find_if(s, not_space));
}

unsigned int Deserializer::valueToUnsignedInt(const std::string& value)
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

    return (unsigned int)numerical_value;
}

unsigned int Deserializer::deserializeParameter(std::istream& input_stream, const std::string& parameter_name)
{
    std::optional<unsigned int> parameter;

    std::string line;
    std::getline(input_stream, line);
    std::istringstream line_stream(line);

    std::string parameter_key;
    if (std::getline(line_stream, parameter_key, kParameterDelimiter))
    {
        trim(parameter_key);

        std::string parameter_value;
        std::getline(line_stream, parameter_value);

        if (parameter_name == parameter_key)
        {
            trim(parameter_value);
            parameter = valueToUnsignedInt(parameter_value);
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

unsigned int Deserializer::deserializeMaxSteps(std::istream& input_stream)
{
    unsigned int max_simulator_steps = deserializeParameter(input_stream, kMaxStepsParameter);

    return max_simulator_steps;
}

std::unique_ptr<Battery> Deserializer::deserializeBattery(std::istream& input_stream)
{
    unsigned int full_battery_capacity = deserializeParameter(input_stream, kMaxBatteryParameter);

    return std::make_unique<Battery>(full_battery_capacity);
}

std::unique_ptr<House> Deserializer::deserializeHouse(std::istream& input_stream)
{
    std::optional<Position> docking_station_position;

    unsigned int house_rows_num = deserializeParameter(input_stream, kHouseRowsNumParameter);
    unsigned int house_cols_num = deserializeParameter(input_stream, kHouseColsNumParameter);

    // Initialize Dirt & Wall maps with their default values
    std::vector<std::vector<bool>> wall_map(house_rows_num, std::vector<bool> (house_cols_num, kDefaultIsWall));
    std::vector<std::vector<unsigned int>> dirt_map(house_rows_num, std::vector<unsigned int> (house_cols_num, kDefaultDirtLevel));

    std::string house_block_row;
    int row_idx = 0;

    while (std::getline(input_stream, house_block_row))
    {
        int column_idx = 0;

        for (char block: house_block_row)
        {
            switch (block)
            {
                case BlockType::DIRT_LEVEL_0:
                case BlockType::DIRT_LEVEL_1:
                case BlockType::DIRT_LEVEL_2:
                case BlockType::DIRT_LEVEL_3:
                case BlockType::DIRT_LEVEL_4:
                case BlockType::DIRT_LEVEL_5:
                case BlockType::DIRT_LEVEL_6:
                case BlockType::DIRT_LEVEL_7:
                case BlockType::DIRT_LEVEL_8:
                case BlockType::DIRT_LEVEL_9:
                    dirt_map[row_idx][column_idx] = (unsigned int)(block - '0');
                    break;

                case BlockType::DOCKING_STATION:
                    if (docking_station_position.has_value())
                    {
                        throw std::runtime_error("More than one docking station was given in house file!");
                    }
                    docking_station_position = {row_idx, column_idx};
                    break;

                case BlockType::WALL:
                    wall_map[row_idx][column_idx] = true;
                    break;

                default:
                    // Space as well as any invalid characters means Clear Block (dirt level of '0')
                    break;
            }
            column_idx++;
        }
        row_idx++;
    }

    if (!docking_station_position.has_value())
    {
        throw std::runtime_error("Missing docking station position in house file!");
    }

    return std::make_unique<House>(
        const_cast<const std::vector<std::vector<bool>>&>(wall_map),
        const_cast<const std::vector<std::vector<unsigned int>>&>(dirt_map),
        const_cast<const Position&>(docking_station_position.value()));
}
