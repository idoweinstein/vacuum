#include "main.h"

#include <cstdlib>
#include <dlfcn.h>
#include <exception>
#include <map>
#include <string>
#include <vector>

#include "common/AlgorithmRegistrar.h"

#include "simulator/robot_logger.h"
#include "simulator/simulator.h"

namespace Constants
{
    using namespace std::string_literals;
    constexpr int kNumberOfArguments = 3;
    constexpr int kAlgoPathArgument = 2;
    constexpr int kHouseFileArgument = 1;

    const std::string&& house_format = ".house"s;

    const std::string&& default_algo_path = ".";
    const std::string&& default_house_path = ".";
    const std::size_t default_num_threads = 10;
    const bool default_summary_only = false;
}

static void openAlgorithms(const std::string &path, std::vector<void*> &algo_handles)
{
    // Open the directory
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr)
    {
        throw std::runtime_error("Failed to open directory: " + path);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string filename = entry->d_name;

        // Skip non-regular and no `.so` suffix files
        if (entry->d_type != DT_REG || filename.length() < 3 || filename.substr(filename.length() - 3) != ".so")
            continue;

        // Construct the full path of the shared object file
        std::string filePath = path + "/" + entry->d_name;

        // Load the shared object file
        void* handle = dlopen(filePath.c_str(), RTLD_NOW);
        if (handle == nullptr)
        {
            RobotLogger::getInstance().addErrorFile(filename.substr(0, filename.length() - 3) + ".error");
            RobotLogger::getInstance().logError(dlerror());
            RobotLogger::getInstance().deleteAllErrorFiles();
            continue;
        }

        algo_handles.push_back(handle);
    }
    closedir(dir);
}

static void closeAlgorithms(std::vector<void*> &algo_handles)
{
    for (void* handle : algo_handles)
    {
        dlclose(handle);
    }
}

static void gethouse_filenames(const std::string &path, std::vector<std::string> &house_filenames)
{
    // Open the directory
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr)
    {
        throw std::runtime_error("Failed to open directory: " + path);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string filename = entry->d_name;

        // Skip non-regular and no `.house` suffix files
        if (entry->d_type != DT_REG 
                || filename.length() < Constants::house_format.size() 
                || filename.substr(filename.length() - Constants::house_format.size()) != Constants::house_format)
        {
            continue;
        }

        // Construct and push the full path of the shared object file
        house_filenames.emplace_back(path + "/" + entry->d_name);
    }
    closedir(dir);
}

static void createSummary(const std::map<std::string, std::map<std::string, std::size_t>>& scores)
{
    std::ofstream summary_file;
    std::string summary_file_name = "summary.csv";

    summary_file.open(summary_file_name);
    if (!summary_file.is_open())
    {
        throw std::runtime_error("Couldn't open summary file");
    }

    // Print header
    summary_file << "algorithm";
    for (const auto& outer_map: scores)
    {
        for (const auto& inner_map: outer_map.second)
        {   
            // Print house name
            summary_file << "," << inner_map.first;
        }
    }
    summary_file << std::endl;

    // Print scores
    for (const auto& outer_map: scores)
    {
        // Print algorithm name
        summary_file << outer_map.first;
        for (const auto& inner_map: outer_map.second)
        {
            // Print score
            summary_file << "," << inner_map.second;
        }
        summary_file << std::endl;
    }

    summary_file.close();
}

void Main::runAll(const Main::Arguments& args)
{
    std::vector<void*> algo_handles;
    std::vector<std::string> house_filenames;
    std::map<std::string, std::map<std::string, std::size_t>> scores;

    openAlgorithms(args.algo_path, algo_handles);
    gethouse_filenames(args.house_path, house_filenames);

    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        std::map<std::string, std::size_t> house_scores;
        for (const auto& house_filename: house_filenames) {
            RobotLogger::getInstance().deleteAllLogFiles();
            std::string house_name = std::filesystem::path(house_filename).filename().replace_extension().string();

            if (!args.summary_only)
            {
                std::string algo_name = algo.name();
                std::cout << "Running " << algo_name << " on " << house_name << std::endl;
                std::string output_file_name = house_name + "-" + algo_name + ".txt";
                RobotLogger::getInstance().addLogFile(output_file_name);
            }

            Simulator simulator;

            try
            {
                simulator.readHouseFile(house_filename);
            }
            catch(const std::exception& exception)
            {
                RobotLogger::getInstance().addErrorFile(house_name + ".error");
                house_scores.insert(std::make_pair(house_filename, -1));
                RobotLogger::getInstance().logError(exception.what());
                RobotLogger::getInstance().deleteAllErrorFiles();
                continue;
            }

            auto algorithm = algo.create();
            simulator.setAlgorithm(*algorithm);

            house_scores.insert(std::make_pair(house_filename, simulator.run()));
        }
        scores.insert(std::make_pair(algo.name(), house_scores));
    }

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    closeAlgorithms(algo_handles);

    createSummary(scores);
}

void printUsage()
{
    std::cout << "Usage: myrobot [-house_path=<path>] [-algo_path=<path>] [-num_threads=<num>] [-summary_only]" << std::endl;
}

bool parseArgument(const std::string& arg, Main::Arguments& args)
{
    if (std::string::npos == arg.find("="))
    {
        printUsage();
        throw std::invalid_argument("Invalid argument: " + arg);
    }

    if (arg.starts_with("-house_path"))
    {
        args.house_path = arg.substr(arg.find("=") + 1);
    }
    else if (arg.starts_with("-algo_path"))
    {
        args.algo_path = arg.substr(arg.find("=") + 1);
    }
    else if (arg.starts_with("-num_threads"))
    {
        args.num_threads = std::stoi(arg.substr(arg.find("=") + 1));
    }
    else if (arg == "-summary_only")
    {
        args.summary_only = true;
    }
    else if (arg.starts_with("-h") || arg.starts_with("-help") || arg.starts_with("--help"))
    {
        printUsage();
        return false;
    }
    else
    {
        printUsage();
        throw std::invalid_argument("Invalid argument: " + arg);
    }

    return true;
}

bool parseAllArguments(int argc, char* argv[], Main::Arguments& args)
{
    for (int i = 1; i < argc; i++)
    {
        if (!parseArgument(argv[i], args))
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[])
{
    RobotLogger& logger = RobotLogger::getInstance();
    Main::Arguments args = {
        .house_path = Constants::default_house_path,
        .algo_path = Constants::default_algo_path,
        .num_threads = Constants::default_num_threads,
        .summary_only = Constants::default_summary_only
    };

    try
    {
        bool run = parseAllArguments(argc, argv, args);
        if (run)
        {
            runAll(args);
        }
    }
    catch(const std::exception& exception)
    {
        logger.logError(exception.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
