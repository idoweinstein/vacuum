#include "gtest/gtest.h"

#include <map>
#include <string>
#include <fstream>
#include <cstddef>
#include <utility>
#include <filesystem>

#include "input_handler.h"
#include "main.h"

namespace
{
    class ThreadingTest : public testing::Test
    {
        inline static constexpr char kTestDirectory = "thread_test_";
        inline static std::size_t test_number = 1;
        std::string test_directory;

        void setUpTestDirectory()
        {
            test_directory = kTestDirectory + std::to_string(test_number);
            std::filesystem::current_path(test_directory);
        }

        void tearDownTestDirectory()
        {
            std::filesystem::current_path("..");
            test_number++;
        }

    protected:
        static const Arguments default_arguments;
        std::map<std::string, std::vector<std::string>> error_files;
        std::map<std::string, std::map<std::string, std::size_t>> score_summary;

        void SetUp() override
        {
            setUpTestDirectory();
        }

        void TearDown() override
        {
            tearDownTestDirectory();
        }

        bool getErrors()
        {
            bool errors_found = false;

            for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
            {
                if (".error" == entry.path().extension())
                {
                    errors_found = true;

                    std::ifstream error_file;

                    error_file.open(entry.path());
                    if (!error_file.is_open())
                    {
                        throw std::runtime_error("Couldn't open error file \"" + entry.path() + "\"");
                        FAIL("Couldn't open an error file!");
                    }

                    std::string error_name = entry.path().stem();
                    error_files[error_name] = {};

                    std::string error_line;

                    while (std::getline(error_file, error_line))
                    {
                        error_files.at(error_name).emplace_back(error_line);
                    }

                    error_file.close();
                }
            }

            return errors_found;
        }

        void getScoreSummary()
        {
            for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
            {
                if ("summary.csv" == entry.path().filename())
                {
                    std::ifstream summary_file;

                    summary_file.open(entry.path());
                    if (!summary_file.is_open())
                    {
                        throw std::runtime_error("Couldn't open summary.csv file!");
                        FAIL("Couldn't open summary file!");
                    }

                    std::string summary_line;
                    std::getline(summary_file, summary_line);
                    std::stringstream first_line(summary_line);
                    std::string house;
                    std::vector<std::string> house_names;

                    // Ignore Header
                    std::string header;
                    std::getline(first_line, header, ',');

                    while (std::getline(first_line, house, ','))
                    {
                        house_names.emplace_back(house);
                    }
                    
                    while (std::getline(summary_file, summary_line))
                    {
                        // Read each line of the file
                        std::stringstream line_stream(summary_line);
                        std::string cell;
                        std::vector<std::string> row;

                        std::string algorithm_name;
                        std::getline(line_stream, algorithm_name, ',');
                        score_summary[algorithm_name] = {};

                        // Split the line by commas
                        std::size_t i = 0;
                        while (std::getline(line_stream, cell, ','))
                        {
                            std::size_t score = static_cast<std::size_t>(std::stoull(cell));
                            score_summary[algorithm_name][house_names.at(i)] = score;
                            i++;
                        }
                    }
                }
            }
        }
    };

    const Arguments ThreadingTest::default_arguments = {
        .house_path = ".",
        .algorithm_path = ".",
        .num_threads = 10,
        .summary_only = false
    };

    TEST_F(ThreadingTest, ThreadingSanity)
    {
        Main::runAll(default_arguments);

        bool is_error = getErrors();
        EXPECT_FALSE(is_error);
    }
}