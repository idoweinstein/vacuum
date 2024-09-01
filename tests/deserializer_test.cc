#include "gtest/gtest.h"

#include <string>
#include <sstream>
#include <streambuf>
#include <filesystem>

#include "simulator/deserializer.h"

namespace
{
    void testErrorMessage(const std::string& input_file_path, const char* expected_error_message)
    {
        try
        {
            HouseFile house_file;
            Deserializer::readHouseFile(std::filesystem::path(input_file_path), house_file);
        }
        catch(const std::runtime_error& error)
        {
            EXPECT_STREQ(expected_error_message, error.what());
        }
        
    }

    TEST(DeserializerTest, InvalidInputFile)
    {
        testErrorMessage("no_way_this_file_exists.txt", "Couldn't open input house file!");
    }

    TEST(DeserializerTest, DeserializerSanity)
    {
        HouseFile house_file;
        Deserializer::readHouseFile(std::filesystem::path("inputs/input_sanity.txt"), house_file);
    }

    TEST(DeserializerTest, InvalidParameters)
    {
        testErrorMessage("inputs/input_invparam.txt", "Missing 'MaxBattery' parameter in house file!");
    }

    TEST(DeserializerTest, MissingParameters)
    {
        testErrorMessage("inputs/input_missparam.txt", "Missing 'MaxBattery' parameter in house file!");
    }

    TEST(DeserializerTest, MissingParameterValue)
    {
        testErrorMessage("inputs/input_missvalue.txt", "A parameter with non-integer value was given!");
    }

    TEST(DeserializerTest, MissingHouse)
    {
        testErrorMessage("inputs/input_nohouse.txt", "Missing docking station position in house file!");
    }

    TEST(DeserializerTest, MissingDockingStation)
    {
        testErrorMessage("inputs/input_nodock.txt", "Missing docking station position in house file!");
    }

    TEST(DeserializerTest, DuplicateDockingStation)
    {
        testErrorMessage("inputs/input_dupdock.txt", "More than one docking station was given in house file!");
    }

    TEST(DeserializerTest, InvalidHouseCharacter)
    {
        HouseFile house_file;
        Deserializer::readHouseFile(std::filesystem::path("inputs/input_invchar.txt"), house_file);
    }
}
