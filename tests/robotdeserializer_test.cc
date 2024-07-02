#include "gtest/gtest.h"

#include "robotdeserializer.h"

namespace
{
    class RobotDeserializerTest : public testing::Test
    {
    };

    TEST_F(RobotDeserializerTest, InvalidInputFile)
    {
        EXPECT_THROW({
            RobotDeserializer::deserializeFromFile("no_way_this_file_exists.txt");
        }, std::runtime_error)
    }
}