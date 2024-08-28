#include "main.h"

#include <map>
#include <string>
#include <vector>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <exception>

#include "common/AlgorithmRegistrar.h"

#include "simulator/simulator.h"

#include "output_handler.h"
#include "input_handler.h"
#include "task_queue.h"
#include "task.h"

namespace Constants
{
    constexpr int kNumberOfArguments = 3;
    constexpr int kAlgoPathArgument = 2;
    constexpr int kHouseFileArgument = 1;

    const std::string kDefaultAlgorithmPath = ".";
    const std::string kDefaultHousePath = ".";
    const std::size_t kDefaultNumThreads = 10;
    const bool kDefaultSummaryOnly = false;
}

void handleResults(TaskQueue& task_queue)
{
    std::map<std::string, std::map<std::string, std::size_t>> task_scores;

    for (auto& task : task_queue)
    {
        // Handle tasks outputs
        OutputHandler::exportStatistics(task.getAlgorithmName(), task.getHouseName(), task.getStatistics());

        // Handle tasks errors
        OutputHandler::exportError(task.getAlgorithmName(), task.getAlgorithmError());

        // Occupy tasks scores
        task_scores[task.getAlgorithmName()].insert(std::make_pair(task.getHouseName(), task.getScore()));

        // Detaching tasks after reading their score as an extra measure for stuck thread (although they should be already cancelled).
        task.detach();
    }

    OutputHandler::exportSummary(task_scores);
}

void runTaskQueue(std::vector<std::filesystem::path>& house_paths, std::size_t num_tasks, std::size_t num_threads)
{
    TaskQueue task_queue(num_tasks, num_threads);

    for(const auto& algorithm : AlgorithmRegistrar::getAlgorithmRegistrar())
    {
        for (const auto& house_path : house_paths)
        {
            task_queue.insertTask(
                algorithm.name(),
                algorithm.create(),
                house_path
            );
        }
    }

    task_queue.run();

    handleResults(task_queue);
}

void Main::runAll(const Arguments& arguments)
{
    std::vector<void*> algorithm_handles;
    std::vector<std::filesystem::path> house_paths;

    InputHandler::openAlgorithms(arguments.algorithm_path, algorithm_handles);
    InputHandler::openHouses(arguments.house_path, house_paths);

    std::size_t num_tasks = algorithm_handles.size() * house_paths.size();

    runTaskQueue(house_paths, num_tasks, arguments.num_threads);

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    InputHandler::closeAlgorithms(algorithm_handles);
}

int main(int argc, char* argv[])
{
    Arguments arguments = {
        .house_path = Constants::kDefaultHousePath,
        .algorithm_path = Constants::kDefaultAlgorithmPath,
        .num_threads = Constants::kDefaultNumThreads,
        .summary_only = Constants::kDefaultSummaryOnly
    };

    try
    {
        bool is_success = InputHandler::parseCmdArguments(argc, argv, arguments);
        if (is_success)
        {
            Main::runAll(arguments);
        }
    }

    catch(const std::exception& exception)
    {
        OutputHandler::printError(exception.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
