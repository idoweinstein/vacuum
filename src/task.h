#ifndef TASK_H_
#define TASK_H_

#include "output_handler.h"

#include "simulator/simulator.h"
#include "simulator/deserializer.h"

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

    // Task Simulation Data
    const std::string& algorithm_name;
    const std::unique_ptr<AbstractAlgorithm> algorithm_pointer;
    const std::string& house_name;
    Simulator simulator;

    // Task Execution Data
    std::jthread worker_thread;
    std::atomic<bool> is_task_ended;
    const std::function<void()> onTeardown;

    // Task Timing Utilities
    std::size_t max_duration;
    boost::asio::io_context& timer_context;

    // Task Results
    std::ostringstream algorithm_error_buffer;
    std::size_t score;

    /**
     * @brief Sets a given thread as an IDLE.
     * Gives the thread the lowest priority possible.
     * 
     * @param thread_handler The thread to set as IDLE.
     */
    static void setIdlePriority(pthread_t& thread_handler);

    /**
     * @brief An handler for a task timeout.
     * 
     * @param error_code The error code of the timeout event.
     * @param task The task the timeout occurred to.
     * @param thread_handler The thread the timeout occurred to.
     */
    static void timeoutHandler(const boost::system::error_code& error_code,
                               Task& task,
                               pthread_t thread_handler);

    /**
     * @brief Adds an error message to the task's error buffer.
     * 
     * @param error_message The error message to be added.
     */
    void setAlgorithmError(const std::string& error_message)
    {
        algorithm_error_buffer << kSimulationError1 << house_name << kSimulationError2 << error_message << std::endl;
    }

    /**
     * @brief Task set-up function to be executed before performing the task.
     * 
     * @param runtime_timer The task's timeout timer.
     */
    void setUpTask(boost::asio::steady_timer& runtime_timer);

    /**
     * @brief Task tear-down function to be executed after performing the task.
     * 
     * @param simulation_score The resultant score of the task (if there's no score std::nullopt).
     * @param runtime_timer The task's timeout timer.
     */
    void tearDownTask(std::optional<std::size_t> simulation_score, boost::asio::steady_timer& runtime_timer);

    /**
     * @brief Simulates an house - algorithm pair.
     * Being executed by the task's distinct worker thread.
     */
    void simulatePair();

public:

    Task(const std::string& algorithm_name,
         std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
         const HouseFile& house_file,
         std::function<void()> onTeardown,
         boost::asio::io_context& timer_context);

    /**
     * @brief Runs the task.
     * Spawns the worker thread, which simulates the task.
     */
    void run() { worker_thread = std::jthread(&Task::simulatePair, this); }

    /**
     * @brief Returns task's simulation score.
     * 
     * @return The score of the simulation.
     */
    std::size_t getScore() const { return score; }

    /**
     * @brief Returns task's simulation statistics report.
     * 
     * @return The simulation statistics report.
     */
    const SimulationStatistics& getStatistics() { return simulator.getSimulationStatistics(); }

    /**
     * @brief Returns task's simulated algorithm name.
     * 
     * @return The simulated algorithm name.
     */
    std::string getAlgorithmName() const { return algorithm_name; }

    /**
     * @brief Returns task's simulated house name.
     * 
     * @return The simulated house name.
     */
    std::string getHouseName() const { return house_name; }

    /**
     * @brief Returns task's algorithm error buffer.
     * 
     * @return The algorithm error buffer.
     */
    std::string getAlgorithmError() const { return algorithm_error_buffer.str(); }

    /**
     * @brief Detaches the task's worker thread.
     */
    void detach()
    {
        if (worker_thread.joinable())
        {
            worker_thread.detach();
        }
    }
};

#endif // TASK_H_