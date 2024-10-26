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

static void handleResults(TaskQueue& task_queue, bool summary_only)
{
    std::map<std::string, std::map<std::string, std::size_t>> task_scores;

    for (auto& task : task_queue)
    {
        if (!summary_only)
        {
            // Handle tasks outputs
            OutputHandler::exportStatistics(task.getAlgorithmName(), task.getHouseName(), task.getStatistics(), task.getScore());
        }

        // Handle tasks errors
        OutputHandler::exportError(task.getAlgorithmName(), task.getAlgorithmError());

        // Occupy tasks scores
        task_scores[task.getAlgorithmName()].insert(std::make_pair(task.getHouseName(), task.getScore()));

        // Detaching tasks after reading their score as an extra measure for stuck thread (although they should be already cancelled).
        task.detach();
    }

    OutputHandler::exportSummary(task_scores);
}

static void runTaskQueue(std::vector<HouseFile>& house_files,
                         std::vector<std::shared_ptr<void>>& algorithm_handles,
                         std::size_t num_tasks, std::size_t num_threads, bool summary_only)
{
    TaskQueue task_queue(num_tasks, num_threads);

    std::size_t i = 0;

    for(const auto& algorithm : AlgorithmRegistrar::getAlgorithmRegistrar())
    {
        for (const auto& house_file : house_files)
        {
            task_queue.insertTask(
                algorithm.name(),
                algorithm.create(),
                house_file,
                algorithm_handles[i]
            );
        }

        ++i;
    }

    task_queue.run();

    handleResults(task_queue, summary_only);
}

void Main::runAll(const Arguments& arguments)
{
    std::vector<std::shared_ptr<void>> algorithm_handles;
    std::vector<std::filesystem::path> house_paths;
    std::vector<HouseFile> house_files;

    try {
        InputHandler::openAlgorithms(arguments.algorithm_path, algorithm_handles);

        InputHandler::findHouses(arguments.house_path, house_paths);
        InputHandler::readHouses(house_paths, house_files);

        std::size_t num_tasks = algorithm_handles.size() * house_files.size();

        runTaskQueue(house_files, algorithm_handles, num_tasks, arguments.num_threads, arguments.summary_only);

        AlgorithmRegistrar::getAlgorithmRegistrar().clear();

    }
    catch(const std::exception& exception)
    {
        // Ensure all instances of the algorithm dealocate before dlclosing even
        // when an exception happened.
        AlgorithmRegistrar::getAlgorithmRegistrar().clear();
        throw;
    }
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
        OutputHandler::printError("Main", exception.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
