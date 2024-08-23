#include "main.h"

#include <cstdlib>
#include <dlfcn.h>
#include <exception>
#include <map>
#include <latch>
#include <string>
#include <vector>
#include <thread>

#include "common/AlgorithmRegistrar.h"

#include "simulator/robot_logger.h"
#include "simulator/simulator.h"

#include "task_queue.h"
#include "task.h"

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

        // Skip directories and hidden files
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

static void createSummary(const std::map<std::string, std::map<std::string, std::optional<std::size_t>>>& scores)
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
            if (inner_map.second.has_value())
            {
                summary_file << "," << inner.map.second.value();
            }

            else
            {
                summary_file << ", TIMEOUT";
            }
        }
        summary_file << std::endl;
    }

    summary_file.close();
}

void Main::runAll(const Main::arguments& args)
{
    std::vector<void*> algorithm_handles;
    std::vector<std::string> house_filenames;

    openAlgorithms(args.algo_path, algorithm_handles);
    gethouse_filenames(args.house_path, house_filenames);

    std::size_t num_of_tasks = algorithm_handles.size() * house_filenames.size();
    std::latch todo_tasks_counter = num_of_tasks;

    std::counting_semaphore active_threads_semaphore(args.num_threads);

    TaskQueue task_queue(todo_tasks_counter, active_threads_semaphore);
    task_queue.reserve(num_of_tasks);

    for(const auto& algorithm: AlgorithmRegistrar::getAlgorithmRegistrar())
    {
        for (const auto& house_name: house_filenames)
        {
            task_queue.insertTask(
                algorithm.name(),
                std::move(algorithm.create()),
                house_name,
                !args.summary_only
            );
        }
    }

    for (auto& task : task_queue)
    {
        active_threads_semaphore.acquire();
        task.run();
    }

    todo_tasks_counter.wait();

    std::map<std::string, std::map<std::string, std::optional<std::size_t>>> scores;
    for (const auto& task : task_queue)
    {
        scores[algorithm_name].insert(std::make_pair(house_name, task.getScore()));
    }

    event_context.stop();
    work_guard.reset();

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    closeAlgorithms(algorithm_handles);

    createSummary(scores);
}

bool parseArgument(const std::string& arg, Main::arguments& args)
{
    if (arg.starts_with("-house_path"))\
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
        std::cout << "Usage: myrobot [-house_path=<path>] [-algo_path=<path>] [-num_threads=<num>] [-summary_only]" << std::endl;
        return false;
    }
    else
    {
        throw std::invalid_argument("Invalid argument: " + arg);
    }

    return true;
}

bool parseArguments(int argc, char* argv[], Main::arguments& args)
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
    Main::arguments args = {
        .house_path = Constants::default_house_path,
        .algo_path = Constants::default_algo_path,
        .num_threads = Constants::default_num_threads,
        .summary_only = Constants::default_summary_only
    };

    try
    {
        bool run = parseArguments(argc, argv, args);
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
