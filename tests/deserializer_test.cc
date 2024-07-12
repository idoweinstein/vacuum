#include "gtest/gtest.h"

#include <string>
#include <sstream>
#include <streambuf>

#include "deserializer.h"
#include "simulator.h"

namespace
{
    class CoutCapture
    {
        std::streambuf* cout_buffer;
        std::ostringstream duplicate_stream;

    public:
        CoutCapture()
        {
            cout_buffer = std::cout.rdbuf();

            std::cout.rdbuf(duplicate_stream.rdbuf());
        }

        ~CoutCapture()
        {
            std::cout.rdbuf(cout_buffer);
        }

        std::string getCoutString()
        {
            return duplicate_stream.str();
        }
    };

    void assertNoWarnings(CoutCapture& cout_capture)
    {
        std::string output = cout_capture.getCoutString();
        EXPECT_STREQ("", output.c_str());
    }

    void assertWarning(CoutCapture& cout_capture, const char* warning_message)
    {
        std::string output = cout_capture.getCoutString();
        EXPECT_STREQ(warning_message, output.c_str());
    }

    TEST(DeserializerTest, InvalidInputFile)
    {
        CoutCapture cout_capture;

        EXPECT_THROW({
            Deserializer::deserializeFromFile("no_way_this_file_exists.txt");
        }, std::runtime_error);

        assertNoWarnings(cout_capture);
    }

    TEST(DeserializerTest, DeserializerSanity)
    {
        CoutCapture cout_capture;

        (void)Deserializer::deserializeFromFile("inputs/input_sanity.txt");

        assertNoWarnings(cout_capture);
    }

    TEST(DeserializerTest, InvalidParameters)
    {
        CoutCapture cout_capture;

        (void)Deserializer::deserializeFromFile("inputs/input_invparam.txt");

        assertWarning(cout_capture, "[WARNING] Invalid configuration parameter was given - Ignoring this line...\n");
    }

    TEST(DeserializerTest, MissingParameters)
    {
        CoutCapture cout_capture;

        (void)Deserializer::deserializeFromFile("inputs/input_missparam.txt");

        assertWarning(cout_capture, "[WARNING] Missing parameters - Initializing missing ones with default value of '0'...\n");
    }

    TEST(DeserializerTest, MissingParameterValue)
    {
        CoutCapture cout_capture;

        (void)Deserializer::deserializeFromFile("inputs/input_missvalue.txt");

        assertWarning(cout_capture, "[WARNING] Parameter with non-integer value given - Setting default value of '0'...\n");
    }

    TEST(DeserializerTest, MissingHouse)
    {
        CoutCapture cout_capture;

        (void)Deserializer::deserializeFromFile("inputs/input_nohouse.txt");

        assertWarning(cout_capture, "[WARNING] House grid is not given - Using an empty house...\n"
                                    "[WARNING] Docking Station was not given - Adding a Docking Station at the end of first row...\n");
    }

    TEST(DeserializerTest, MissingDockingStation)
    {
        CoutCapture cout_capture;

        (void)Deserializer::deserializeFromFile("inputs/input_nodock.txt");

        assertWarning(cout_capture, "[WARNING] Docking Station was not given - Adding a Docking Station at the end of first row...\n");
    }

    TEST(DeserializerTest, DuplicateDockingStation)
    {
        CoutCapture cout_capture;

        (void)Deserializer::deserializeFromFile("inputs/input_dupdock.txt");

        assertWarning(cout_capture, "[WARNING] Docking Station defined more than once - Using first definition...\n");
    }

    TEST(DeserializerTest, InvalidHouseCharacter)
    {
        CoutCapture cout_capture;

        (void)Deserializer::deserializeFromFile("inputs/input_invchar.txt");

        assertWarning(cout_capture, "[WARNING] Invalid character given in House - Parsing it as a wall...\n");
    }
}
