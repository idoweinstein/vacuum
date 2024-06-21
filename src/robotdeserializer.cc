#include <fstream>

#include "robotdeserializer.h"

void RobotDeserializer::storeParameter(int* parameters, const std::string& key, int value)
{
    if (0 == key.compare("max_battery_steps"))
    {
        parameters[Parameter::MAX_BATTERY_STEPS] = value;
    }

    if (0 == key.compare("max_robot_steps"))
    {
        parameters[Parameter::MAX_ROBOT_STEPS] = value;
    }
}

void RobotDeserializer::deserializeParameters(int* parameters, std::istream& input_stream)
{
    for (int i = 0; i < robot.kNumberOfParameters; i++)
    {
        string line;
        std::getline(input_file, line);
        std::istringstream line_stream(line);

        std::string key;
        if (std::getline(line_stream, key, ' '))
        {
            std::string value;
            std::getline(line_stream, value);

            RobotDeserializer::storeParameter(parameters, key, std::stoi(value));
        }
    }
}

void RobotDeserializer::deserializeHouse(std::vector<std::vector<bool>>& wall_map,
                                         std::vector<std::vector<unsigned int>>& dirt_map,
                                         std::pair<unsigned int, unsigned int>& docking_station_position,
                                         std::istream& input_stream)
{
    string house_block_row;
    unsigned int row_idx = 0;
    while (std::getline(input_file, house_block_row))
    {
        unsigned int column_idx = 0;
        for (char block: house_block_row)
        {
            switch (block)
            {
                case BlockType::DOCKING_STATION:
                    docking_station_position = {row_idx, column_idx};
                    break;
                
                case BlockType::WALL:
                    wall_map[row_idx][column_idx] = true;
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
            }
            column_idx++;
        }
        row_idx++;
    }
}

void RobotDeserializer::deserializeFromFile(Robot& robot,
                                            std::vector<std::vector<bool>>& wall_map,
                                            std::vector<std::vector<unsigned int>>& dirt_map,
                                            const std::string& input_file_path)
{
    std::ifstream input_file(input_file_path);

    int parameters[] = int[sizeof(Parameter)];
    deserializeParameters(parameters, input_file);

    std::pair<unsigned int, unsigned int> docking_station_position;
    deserializeHouse(wall_map, dirt_map, docking_station_position, input_file);

    robot = Robot(parameters[Parameter::MAX_BATTERY_STEPS],
                  parameters[Parameter::MAX_ROBOT_STEPS],
                  wall_map,
                  dirt_map,
                  docking_station_position);
}