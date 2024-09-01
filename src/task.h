#ifndef TASK_H_
#define TASK_H_

#include "output_handler.h"

#include "simulator/simulator.h"
#include "common/AlgorithmRegistrar.h"

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <pthread.h>

#include <optional>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

/**
 * Credits to Amir's demonstration
 */
class Task
{
    // Task Constants
    inline static const char kSimulationError1[] = "[house=";
    inline static const char kSimulationError2 = ']';

    bool is_runnable; // Whether or not Task is runnable

    // Task Simulation Data
    const std::string& algorithm_name;
    const std::unique_ptr<AbstractAlgorithm> algorithm_pointer;
    const std::string house_name;
    Simulator simulator;

    // Task Execution Data
    std::jthread executing_thread;
    std::atomic<bool> is_task_ended;
    const std::function<void()> onTeardown;

    // Task Timing Utilities
    std::size_t max_duration;
    boost::asio::io_context& timer_context;

    // Task Results
    std::ostringstream algorithm_error_buffer;
    std::size_t score;

    static void setIdlePriority(pthread_t& thread_handler);

    static void timeoutHandler(const boost::system::error_code& error_code,
                               Task& task,
                               pthread_t thread_handler);

    void setAlgorithmError(const std::string& error_message)
    {
        algorithm_error_buffer << kSimulationError1 << house_name << kSimulationError2 << error_message << std::endl;
    }

    void setUpTask(boost::asio::steady_timer& runtime_timer);

    void tearDownTask(std::optional<std::size_t> simulation_score, boost::asio::steady_timer& runtime_timer);

    void simulatePair();

public:

    Task(const std::string& algorithm_name,
         std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
         const std::filesystem::path& house_path,
         std::function<void()> onTeardown,
         boost::asio::io_context& timer_context);

    bool isRunnable() { return is_runnable; }

    void run() { executing_thread = std::jthread(&Task::simulatePair, this); }

    std::size_t getScore() const { return score; }

    SimulationStatistics& getStatistics() { return simulator.getSimulationStatistics(); }

    std::string getAlgorithmName() const { return algorithm_name; }

    std::string getHouseName() const { return house_name; }

    std::string getAlgorithmError() const { return algorithm_error_buffer.str(); }

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