#ifndef VACUUM_ROBOTDESERIALIZER_H_
#define VACUUM_ROBOTDESERIALIZER_H_

#include <map>
#include <vector>
#include <string>
#include <istream>
 
#include "position.h"
#include "robot.h"

class RobotDeserializer
{
    enum BlockType : char
    {
        DOCKING_STATION = '@',
        WALL = 'X',
        EMPTY = ' ',
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

    static unsigned int valueToUnsignedInt(const std::string& value);
    static bool storeParameter(unsigned int* parameters, const std::string& key, const std::string& value);
    static void deserializeParameters(unsigned int* parameters, std::istream& input_stream);
    static void deserializeHouse(std::vector<std::vector<bool>>& wall_map,
                                 std::vector<std::vector<unsigned int>>& dirt_map,
                                 Position& docking_station_position,
                                 std::istream& input_stream);

public:
    static Robot deserializeFromFile(const std::string& input_file_name);
};

#endif /* VACUUM_DESERIALIZER_H_ */
