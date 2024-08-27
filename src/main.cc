#include "main.h"

#include <cstdlib>
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
    constexpr int kNumberOfArguments = 3;
    constexpr int kAlgoPathArgument = 2;
    constexpr int kHouseFileArgument = 1;

    const std::string& default_algorithm_path = ".";
    const std::string& default_house_path = ".";
    const std::size_t default_num_threads = 10;
    const bool default_summary_only = false;
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

void Main::runAll(const Arguments& arguments)
{
    std::vector<void*> algorithm_handles;
    std::vector<std::string> house_filenames;

    InputHandler::openAlgorithms(arguments.algorithm_path, algorithm_handles);
    InputHandler::openHouses(arguments.house_path, house_filenames);

    std::cout << "Finished opening algorithms and houses successfuly!!!" << std::endl;
    std::cout << "Number of algorithms=" << algorithm_handles.size() << std::endl;
    std::cout << "Number of houses=" << house_filenames.size() << std::endl;

    std::size_t num_of_tasks = algorithm_handles.size() * house_filenames.size();

    TaskQueue task_queue(num_of_tasks, arguments.num_threads);

    for(const auto& algorithm: AlgorithmRegistrar::getAlgorithmRegistrar())
    {
        for (auto house_name: house_filenames)
        {
            task_queue.insertTask(
                algorithm.name(),
                algorithm.create(),
                house_name
            );
        }
    }

    std::cout << "-------------------------------------------" << std::endl;
    std::cout << "Finished occupying the Tasks Queue!!!" << std::endl;

    task_queue.run();

    std::cout << "-------------------------------------------" << std::endl;
    std::cout << "Finished running Tasks Queue!!!" << std::endl;

    handleResults(task_queue);

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    InputHandler::closeAlgorithms(algorithm_handles);
}

int main(int argc, char* argv[])
{
    Arguments arguments = {
        .house_path = Constants::default_house_path,
        .algorithm_path = Constants::default_algorithm_path,
        .num_threads = Constants::default_num_threads,
        .summary_only = Constants::default_summary_only
    };

    try
    {
        bool run = InputHandler::parseCmdArguments(argc, argv, arguments);
        if (run)
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
