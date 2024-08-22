#include "simulator/simulator.h"

#include <optional>
#include <pthread>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>

using time_point = std::chrono::time_point<std::chrono::system_clock>;

/**
 * Credits to Amir's demonstration
 */
class Task
{
    Simulator simulator;

    std::atomic<bool> is_task_ended; // Indicates whether task was naturally finished or terminated by a timeout
    std::latch& tasks_left_counter;

    std::size_t& running_threads_num;
    std::mutex& running_threads_mutex;

    boost::asio::io_context& event_context;
    std::size_t max_duration;
    std::jthread executing_thread;
    std::optional<std::size_t> score;

    static void simulationTimeoutHandler(boost::system::error_code& error_code,
                                         Task& task,
                                         time_point& start_time,
                                         pthread_t thread_handler)
    {
        // Make sure timer was not cancelled.
        if (boost::asio::error::operation_aborted != error_code)
        {
            time_point current_time = std::chrono::system_clock::now();
            time_point run_duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);

            bool expected_value = false;
            bool is_simulation_timeout = task.is_task_ended.compare_exchange_strong(expected_value, true);
            if (is_simulation_timeout)
            {
                pthread_cancel(thread_handler);
                task.tasks_left_counter.count_down();
            }
        }
    }

public:
    Task(std::unique_ptr<AbstractAlgorithm>&& algorithm, std::string& house_filename, std::latch& tasks_left_counter, boost::asio::io_context& event_context, bool is_logging, std::size_t& running_threads_num, std::mutex& running_threads_mutex)
        : tasks_left_counter(tasks_left_counter), event_context(event_context), max_duration(max_duration), running_threads_num(running_threads_num), running_threads_mutex(running_threads_mutex)
    {
        simulator.readHouseFile(house_filename, is_logging);
        simulator.setAlgorithm(*algorithm);

        max_duration = std::chrono::milliseconds(1) * simulator.getMaxSteps();
    }

    void run()
    {
        auto simulatePair = [this]() {
            // Make this thread asynchronously cancel-able from another thread
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
            pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);

            boost::asio::steady_timer runtime_timer(event_context, std::chrono:milliseconds(max_duration));
            time_point start_time = std::chrono::system_clock::now();
            runtime_timer.async_wait([&](const boost::system::error_code& error_code) {
                simulationTimeoutHandler(error_code, *this, start_time, pthread_self())
            });

            // Actual task
            // TODO: Think what to do about the logging - each thread need to log into a separate file
            std::size_t simulation_score = simulator.run();

            runtime_timer.cancel();

            bool expected_value = false;
            bool is_simulation_finished = is_task_ended.compare_exchange_strong(expected_value, true);
            if (is_simulation_finished)
            {
                // Simulation finished successuly with NO timeout
                score = simulation_score;
                tasks_left_counter.count_down();
            }

            std::lock_guard<std::mutex> lock_guard(running_threads_mutex);
            running_threads_num--;
        };

        executing_thread = std::jthread(simulatePair);
    }

    std::optional<std::size_t> getScore() const
    {
        return score;
    }

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