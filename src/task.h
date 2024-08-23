#ifndef TASK_H_
#define TASK_H_

#include "task_queue.h"
#include "simulator/simulator.h"
#include "common/AlgorithmRegistrar.h"

#include <optional>
#include <pthread>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

using time_point = std::chrono::time_point<std::chrono::system_clock>;
using namespace std::chrono_literals;

/**
 * Credits to Amir's demonstration
 */
class Task
{
    TaskQueue& task_queue;

    std::string& algorithm_name;
    std::unique_ptr<AbstractAlgorithm> algorithm_pointer;
    std::string& house_name;
    Simulator simulator;

    std::size_t max_duration;
    std::jthread executing_thread;
    std::atomic<bool> is_task_ended; // Indicates whether task was naturally finished or terminated by a timeout
    std::optional<std::size_t> score;

    static void timeoutHandler(boost::system::error_code& error_code,
                               Task& task,
                               time_point& start_time,
                               pthread_t thread_handler);

public:
    Task(TaskQueue& task_queue,
         std::string& algorithm_name,
         std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
         std::string& house_file_name,
         bool is_logging);

    void setUpTask();

    void tearDownTask(std::size_t simulation_score);

    void simulatePair()
    {
        setUpTask();
    
        // Actual Task
        // TODO: Think what to do about the logging - each thread need to log into a separate file
        std::size_t simulation_score = simulator.run();

        tearDownTask(simulation_score);
    }

    void run() { executing_thread = std::jthread(&Task::simulatePair, this); }

    std::optional<std::size_t> getScore() const { return score; }

    std::string getAlgorithmName() const { return algorithm_name; }

    std::string getHouseName() const { return house_name; }

    void join()
    {
        if (executing_thread.joinable())
        {
            executing_thread.join();
        }
    }

    void detach()
    {
        if (executing_thread.joinable())
        {
            executing_thread.detach();
        }
    }
};

#endif // TASK_H_