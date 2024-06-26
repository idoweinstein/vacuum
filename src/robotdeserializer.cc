#include <stdexcept>
#include <fstream>
#include <sstream>

#include "robot.h"
#include "position.h"
#include "robotlogger.h"
#include "robotdeserializer.h"

const std::map<std::string, RobotDeserializer::Parameter> RobotDeserializer::parameter_map = {
        {"max_battery_steps", Parameter::MAX_BATTERY_STEPS},
        {"max_robot_steps", Parameter::MAX_ROBOT_STEPS}
};

void RobotDeserializer::storeParameter(unsigned int* parameters, const std::string& key, unsigned int value)
{
    if (parameter_map.contains(key))
    {
        unsigned int parameter_index = parameter_map.at(key);
        parameters[parameter_index] = value;
    }

    else
    {
        RobotLogger::logWarning("Invalid configuration parameter was given - Ignoring this line");
    }
}

void RobotDeserializer::deserializeParameters(unsigned int* parameters, std::istream& input_stream)
{
    for (int i = 0; i < Parameter::NUMBER_OF_PARAMETERS; i++)
    {
        std::string line;
        std::getline(input_stream, line);
        std::istringstream line_stream(line);

        std::string key;
        if (std::getline(line_stream, key, kParameterDelimiter))
        {
            std::string value;
            std::getline(line_stream, value);

            RobotDeserializer::storeParameter(parameters, key, (unsigned int)std::stoi(value));
        }
    }
}

void RobotDeserializer::deserializeHouse(std::vector<std::vector<bool>>& wall_map,
                                         std::vector<std::vector<unsigned int>>& dirt_map,
                                         UPosition& docking_station_position,
                                         std::istream& input_stream)
{
    std::string house_block_row;
    unsigned int row_idx = 0;

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
                    docking_station_position = {row_idx, column_idx};
                    break;

                case BlockType::WALL:
                    wall_map[row_idx][column_idx] = true;
                    break;

                default:   /* We've decided to translate any other given character as a wall */
                    wall_map[row_idx][column_idx] = true;
                    RobotLogger::logWarning("Invalid character was given in house table - Parsing as a wall.");
                    break;
            }
            column_idx++;
        }
        row_idx++;
    }
}

Robot RobotDeserializer::deserializeFromFile(std::vector<std::vector<bool>>& wall_map,
                                             std::vector<std::vector<unsigned int>>& dirt_map,
                                             const std::string& input_file_name)
{
    std::ifstream input_file;
    input_file.open(input_file_name);

    if (!input_file.is_open())
    {
        throw std::runtime_error("Couldn't open input file!");
    }

    unsigned int parameters[Parameter::NUMBER_OF_PARAMETERS] = {0};
    deserializeParameters(parameters, input_file);

    UPosition docking_station_position;
    deserializeHouse(wall_map, dirt_map, docking_station_position, input_file);

    Robot robot(parameters[Parameter::MAX_ROBOT_STEPS],
                parameters[Parameter::MAX_BATTERY_STEPS],
                wall_map,
                dirt_map,
                docking_station_position);

    return robot;
}