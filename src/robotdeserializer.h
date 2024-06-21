#ifndef VACUUM_ROBOTDESERIALIZER_H_
#define VACUUM_ROBOTDESERIALIZER_H_

#include <istream>
#include <utility>
#include <vector>
#include <string>
 
#include "robot.h"

class RobotDeserializer
{
    enum BlockType : char
    {
        DOCKING_STATION = 'o',
        WALL = 'x',
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
        MAX_BATTERY_STEPS = 0,
        MAX_ROBOT_STEPS
    };

    const int kNumberOfParameters = 2;
    const char kParameterDelimiter = ' ';

    static void storeParameter(unsigned int* parameters, const std::string& key, unsigned int value);
    static void deserializeParameters(unsigned int* parameters, std::istream& input_stream);
    static void deserializeHouse(std::vector<std::vector<bool>>& wall_map,
                                 std::vector<std::vector<unsigned int>>& dirt_map,
                                 std::pair<unsigned int, unsigned int>& docking_station_position,
                                 std::istream& input_stream);
public:
    static Robot deserializeFromFile(std::vector<std::vector<bool>>& wall_map,
                                    std::vector<std::vector<unsigned int>>& dirt_map,
                                    const std::string& input_file_path);
};

#endif /* VACUUM_DESERIALIZER_H_ */