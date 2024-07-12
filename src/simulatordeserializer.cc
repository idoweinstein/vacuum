#include "simulatordeserializer.h"

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "batterycontroller.h"
#include "locationmanager.h"
#include "robotlogger.h"
#include "position.h"

unsigned int SimulatorDeserializer::valueToUnsignedInt(const std::string& value)
{
    RobotLogger& logger = RobotLogger::getInstance();
    std::istringstream value_stream(value);
    int numerical_value = 0;

    value_stream >> numerical_value;
    if (!value_stream)
    {
        logger.logWarning("Parameter with non-integer value given - Setting default value of '0'...");
        numerical_value = kDefaultParameterValue;
    }

    if (numerical_value < 0)
    {
        logger.logWarning("Parameter with negative value given - Setting default value of '0'...");
        numerical_value = kDefaultParameterValue;
    }

    return (unsigned int)numerical_value;
}

unsigned int SimulatorDeserializer::deserializeParameter(std::istream& input_stream, const std::string& parameter_name)
{
    Parameter parameter;

    std::string line;
    std::getline(input_stream, line);
    std::istringstream line_stream(line);

    std::string parameter_key;
    if (std::getline(line_stream, parameter_key, kParameterDelimiter))
    {
        std::string parameter_value;
        std::getline(line_stream, parameter_value);

        if (parameter_name == parameter_key)
        {
            parameter.is_initialized = true;
            parameter.value = valueToUnsignedInt(parameter_value);
        }
    }

    assertParameterSet(parameter, parameter_name);

    return paramter.value;
}

unsigned int SimulatorDeserializer::deserializeMaxSteps(std::istream& input_stream)
{
    // Unused input line - used for internal naming
    std::string house_internal_name;
    std::getline(input_stream, house_internal_name);

    unsigned int max_simulator_steps = deserializeParameter(input_stream, kMaxStepsParameter);

    return max_simulator_steps;
}

std::unique_ptr<BatteryController> SimulatorDeserializer::deserializeBattery(std::istream& input_stream)
{
    unsigned int full_battery_capacity = deserializeParameter(input_stream, kMaxBatteryParameter);

    return make_unique<BatteryController>(full_battery_capacity);
}

std::unique_ptr<LocationManager> SimulatorDeserializer::deserializeHouse(std::istream& input_stream)
{
    std::vector<std::vector<bool>> wall_map;
    std::vector<std::vector<unsigned int>> dirt_map;
    DockingStation docking_station;

    unsigned int house_rows_num = deserializeParameter(input_stream, kHouseRowsNumParameter);
    unsigned int house_cols_num = deserializeParameter(input_stream, kHouseColsNumParameter);

    // Initialize Dirt & Wall maps with their default values
    wall_map.insert(0, house_rows_num, {});
    dirt_map.insert(0, house_rows_num, {});
    for (unsigned int i = 0; i < house_cols_num; i++)
    {
        wall_map.at(i).insert(0, house_cols_num, kDefaultIsWall);
        dirt_map.at(i).insert(0, house_cols_num, kDefaultDirtLevel);
    }

    std::string house_block_row;
    unsigned int row_idx = 0;

    while (std::getline(input_stream, house_block_row))
    {
        unsigned int column_idx = 0;

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
                    if (docking_station.is_initialized)
                    {
                        throw std::runtime_error("More than one docking station was given in house file!");
                    }
                    docking_station.position = {row_idx, column_idx};
                    docking_station.is_initialized = true;
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

    if (!docking_station.is_initialized)
    {
        throw std::runtime_error("Missing docking station position in house file!");
    }

    return std::make_unique<LocationManager>(wall_map, dirt_map, docking_station_position);
}
