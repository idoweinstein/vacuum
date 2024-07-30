#include "main.h"

#include <string>
#include <cstdlib>
#include <exception>
#include <dlfcn.h>
#include <vector>

#include "common/AlgorithmRegistrar.h"

#include "simulator/robot_logger.h"
#include "simulator/simulator.h"

namespace Constants
{
    constexpr int kNumberOfArguments = 3;
    constexpr int kAlgoPathArgument = 2;
    constexpr int kHouseFileArgument = 1;
}

static void openAlgorithms(const std::string &path, std::vector<void*> &algoHandles)
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

        // Skip directories and hidden files
        if (entry->d_type != DT_REG || filename.length() < 3 || filename.substr(filename.length() - 3) != ".so")
            continue;

        // Construct the full path of the shared object file
        std::string filePath = path + "/" + entry->d_name;

        // Load the shared object file
        void* handle = dlopen(filePath.c_str(), RTLD_NOW);
        if (handle == nullptr)
        {
            RobotLogger::getInstance().logError(dlerror());
            continue;
        }

        algoHandles.push_back(handle);
    }
    closedir(dir);
}

static void closeAlgorithms(std::vector<void*> &algoHandles)
{
    for (void* handle : algoHandles)
    {
        dlclose(handle);
    }
}

static void getHouseFilenames(const std::string &path, std::vector<std::string> &houseFilenames)
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

        // Skip directories and hidden files
        if (entry->d_type != DT_REG || filename.length() < 6 || filename.substr(filename.length() - 6) != ".house")
            continue;

        // Construct and push the full path of the shared object file
        houseFilenames.emplace_back(path + "/" + entry->d_name);
    }
    closedir(dir);
}

void Main::runAll(const Main::arguments& args)
{
    std::vector<void*> algoHandles;
    std::vector<std::string> houseFilenames;

    openAlgorithms(args.algo_path, algoHandles);
    getHouseFilenames(args.house_path, houseFilenames);

    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        for (const auto& houseFilename: houseFilenames) {
            RobotLogger::getInstance().deleteAllLogFiles();

            Simulator simulator;

            std::cout << "Running " << algo.name() << " on " << houseFilename << std::endl;

            simulator.readHouseFile(houseFilename);

            auto algorithm = algo.create();
            simulator.setAlgorithm(*algorithm);

            simulator.run();
        }
    }

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    closeAlgorithms(algoHandles);
}

int main(int argc, char* argv[])
{
    RobotLogger& logger = RobotLogger::getInstance();
    Main::arguments args = {
        .house_path = ".",     // Default house path - current working directory
        .algo_path = ".",      // Default algo path - current working directory
        .num_threads = 10,     // Default number of threads
        .summary_only = false  // Default summary option - show all logs
    };

    if (Constants::kNumberOfArguments == argc)
    {
        args.house_path = argv[Constants::kHouseFileArgument];
        args.algo_path = argv[Constants::kAlgoPathArgument];

        try
        {
            runAll(args);
        }
        catch(const std::exception& exception)
        {
            logger.logError(exception.what());
            return EXIT_FAILURE;
        }
    }

    else
    {
        logger.logError("Invalid number of arguments!\nUsage: myrobot <house_path> <algo_path>");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
