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

#include "simulator/simulator.h"

#include "output_handler.h"
#include "input_handler.h"
#include "task_queue.h"
#include "task.h"

namespace Constants
{
    using namespace std::string_literals;
    constexpr int kNumberOfArguments = 3;
    constexpr int kAlgoPathArgument = 2;
    constexpr int kHouseFileArgument = 1;

    const std::string& algorithm_format = ".so"s;
    const std::string& house_format = ".house"s;

    const std::string& default_algo_path = ".";
    const std::string& default_house_path = ".";
    const std::size_t default_num_threads = 10;
    const bool default_summary_only = false;
}

void summarizeScores(TaskQueue& task_queue)
{
    // Extract Tasks Scores
    std::map<std::string, std::map<std::string, std::size_t>> scores;
    for (auto& task : task_queue)
    {
        scores[task.getAlgorithmName()].insert(std::make_pair(task.getHouseName(), task.getScore()));
        // Detaching tasks after reading their score as an extra measure for stuck thread (although they should be already cancelled).
        task.detach();
    }

    OutputHandler::exportSummary(scores);
}

void handleResults(TaskQueue& task_queue)
{
    std::map<std::string, std::map<std::string, std::size_t>> task_scores;

    for (auto& task : task_queue)
    {
        // Handle tasks outputs
        OutputHandler::exportStatistics(task.getAlgorithmName(), task.getHouseName(), task.getStatistics());

        // Handle tasks errors
        OutputHandler::exportErrors(task.getAlgorithmName(), task.getHouseName(), );

        // Occupy tasks scores
        scores[task.getAlgorithmName()].insert(std::make_pair(task.getHouseName(), task.getScore()));

        // Detaching tasks after reading their score as an extra measure for stuck thread (although they should be already cancelled).
        task.detach();
    }

    OutputHandler::exportSummary(task_scores);
}

void Main::runAll(const Arguments& arguments)
{
    std::vector<void*> algorithm_handles;
    std::vector<std::string> house_filenames;

    InputHandler::openAlgorithms(arguments.algorithm_path, algorithm_handles);
    InputHandler::openHouses(arguments.house_path, house_filenames);

    std::size_t num_of_tasks = algorithm_handles.size() * house_filenames.size();

    TaskQueue task_queue(num_of_tasks, args.num_threads);

    for(const auto& algorithm: AlgorithmRegistrar::getAlgorithmRegistrar())
    {
        for (const auto& house_name: house_filenames)
        {
            task_queue.insertTask(
                algorithm.name(),
                std::move(algorithm.create()),
                house_name,
                !arguments.summary_only
            );
        }
    }

    task_queue.run();

    handleResults(task_queue);

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    InputHandler::closeAlgorithms(algorithm_handles);
}

int main(int argc, char* argv[])
{
    Arguments arguments = {
        .house_path = Constants::default_house_path,
        .algorithm_path = Constants::default_algo_path,
        .num_threads = Constants::default_num_threads,
        .summary_only = Constants::default_summary_only
    };

    try
    {
        bool run = InputHandler::parseCmdArguments(argc, argv, arguments);
        if (run)
        {
            runAll(arguments);
        }
    }

    catch(const std::exception& exception)
    {
        OutputHandler::printError(exception.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
