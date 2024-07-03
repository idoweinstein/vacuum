#include "robotdeserializer.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "robotlogger.h"
#include "position.h"
#include "robot.h"

const std::map<std::string, RobotDeserializer::Parameter> RobotDeserializer::parameter_map = {
        {"max_battery_steps", Parameter::MAX_BATTERY_STEPS},
        {"max_robot_steps", Parameter::MAX_ROBOT_STEPS},
        {"house", Parameter::END_OF_PARAMETER}
};

unsigned int RobotDeserializer::valueToUnsignedInt(const std::string& value)
{
    RobotLogger& logger = RobotLogger::getInstance();
    std::istringstream value_stream(value);
    int numerical_value = 0;

    value_stream >> numerical_value;
    if (!value_stream)
    {
        logger.logWarning("Parameter with non-integer value given - Setting default value of '0'...");
        numerical_value = 0;
    }

    if (numerical_value < 0)
    {
        logger.logWarning("Parameter with negative value given - Setting default value of '0'...");
        numerical_value = 0;
    }

    return numerical_value;
}

bool RobotDeserializer::storeParameter(unsigned int* parameters, const std::string& key, const std::string& value)
{
    RobotLogger& logger = RobotLogger::getInstance();

    if (parameter_map.contains(key))
    {
        Parameter parameter_type = parameter_map.at(key);

        if (Parameter::END_OF_PARAMETER == parameter_type)
        {
            return true;
        }

        unsigned int parameter_index = (unsigned int) parameter_type;
        parameters[parameter_index] = valueToUnsignedInt(value);
    }

    else
    {
        logger.logWarning("Invalid configuration parameter was given - Ignoring this line...");
    }

    return false;
}

void RobotDeserializer::deserializeParameters(unsigned int* parameters, std::istream& input_stream)
{
    std::string line;
    while (std::getline(input_stream, line))
    {
        std::istringstream line_stream(line);

        std::string key;
        if (std::getline(line_stream, key, kParameterDelimiter))
        {
            std::string value;
            std::getline(line_stream, value);
            
            bool end_of_parameters = RobotDeserializer::storeParameter(parameters, key, value);
            if (end_of_parameters)
            {
                break;
            }
        }
    }
}

void RobotDeserializer::deserializeHouse(std::vector<std::vector<bool>>& wall_map,
                                         std::vector<std::vector<unsigned int>>& dirt_map,
                                         Position& docking_station_position,
                                         std::istream& input_stream)
{
    RobotLogger& logger = RobotLogger::getInstance();
    std::string house_block_row;
    unsigned int row_idx = 0;
    bool is_docking_station_initialized = false;

    while (std::getline(input_stream, house_block_row))
    {
        wall_map.push_back({});
        dirt_map.push_back({});
        unsigned int column_idx = 0;

        for (char block: house_block_row)
        {
            wall_map[row_idx].push_back(false);
            dirt_map[row_idx].push_back(0);

            switch (block)
            {                
                // Empty block (space character) means Dirt Level 0
                case BlockType::EMPTY:
                    dirt_map[row_idx][column_idx] = 0;
                    break;

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
                    if (is_docking_station_initialized)
                    {
                        logger.logWarning("Docking Station defined more than once - Using latest definition...");
                    }
                    docking_station_position = {(int)row_idx, (int)column_idx};
                    is_docking_station_initialized = true;
                    break;

                case BlockType::WALL:
                    wall_map[row_idx][column_idx] = true;
                    break;

                default:   /* We've decided to translate any other given character as a wall */
                    wall_map[row_idx][column_idx] = true;
                    logger.logWarning("Invalid character given in House - Parsing it as a wall...");
                    break;
            }
            column_idx++;
        }
        row_idx++;
    }
}

Robot RobotDeserializer::deserializeFromFile(const std::string& input_file_name)
{
    std::ifstream input_file;
    input_file.open(input_file_name);

    if (!input_file.is_open())
    {
        throw std::runtime_error("Couldn't open input file!");
    }

    unsigned int parameters[Parameter::NUMBER_OF_PARAMETERS] = {0};
    deserializeParameters(parameters, input_file);

    Position docking_station_position;
    std::vector<std::vector<bool>> wall_map;
    std::vector<std::vector<unsigned int>> dirt_map;
    deserializeHouse(wall_map, dirt_map, docking_station_position, input_file);

    Robot robot(parameters[Parameter::MAX_ROBOT_STEPS],
                parameters[Parameter::MAX_BATTERY_STEPS],
                wall_map,
                dirt_map,
                docking_station_position);

    return robot;
}