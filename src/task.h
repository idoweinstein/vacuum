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
    bool is_runnable;

    const std::string& algorithm_name;
    const std::unique_ptr<AbstractAlgorithm> algorithm_pointer;
    const std::string house_name;
    Simulator simulator;
    std::size_t score;

    std::size_t max_duration;
    std::jthread executing_thread;
    std::atomic<bool> is_task_ended;
    boost::asio::steady_timer runtime_timer;

    const std::function<void()> on_teardown;

    std::ostringstream algorithm_error_buffer;

    static void timeoutHandler(const boost::system::error_code& error_code,
                               Task& task,
                               pthread_t thread_handler);

public:

    Task(const std::string& algorithm_name,
         std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
         const std::filesystem::path& house_path,
         boost::asio::io_context& timer_event_context,
         std::function<void()> on_teardown);

    void setUpTask();

    void tearDownTask(std::optional<std::size_t> simulation_score);

    void simulatePair()
    {
        setUpTask();

        std::optional<std::size_t> simulation_score;
        // Actual Task
        try
        {
            simulation_score = simulator.run();
        }
        catch(const std::exception& exception)
        {
            algorithm_error_buffer << "[House=" << house_name << "]" << exception.what() << std::endl;
        }

        tearDownTask(simulation_score);
    }

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