#include "task.h"

void Task::timeoutHandler(const boost::system::error_code& error_code,
                          Task& task,
                          [[maybe_unused]] time_point& start_time,
                          pthread_t thread_handler)
{
    // Make sure timer was not cancelled.
    if (boost::asio::error::operation_aborted != error_code) // TODO: Make sure it's correct
    {
        bool expected_value = false;
        bool is_simulation_timeout = task.is_task_ended.compare_exchange_strong(expected_value, true);
        if (is_simulation_timeout)
        {
            task.score = task.simulator.getTimeoutScore();
            pthread_cancel(thread_handler);
            task.task_ended_();
        }
    }
}

Task::Task(boost::asio::io_context& timer_event_context,
           std::function<void()>&& task_ended,
           const std::string& algorithm_name,
           std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
           const std::string& house_name,
           bool is_logging)
            : runtime_timer(timer_event_context),
              algorithm_name(algorithm_name),
              algorithm_pointer(std::move(algorithm_pointer)),
              house_name(house_name),
              is_task_ended(false),
              task_ended_(std::move(task_ended))
{
    simulator.readHouseFile(house_name, is_logging);
    simulator.setAlgorithm(*algorithm_pointer);

    max_duration = simulator.getMaxSteps();
}

Task::Task(Task&& other) noexcept
    : runtime_timer(std::move(other.runtime_timer)),
      algorithm_name(std::move(other.algorithm_name)),
      algorithm_pointer(std::move(other.algorithm_pointer)),
      house_name(std::move(other.house_name)),
      is_task_ended(false),
      task_ended_(std::move(other.task_ended_))
{}

void Task::setUpTask()
{
    // Make this thread asynchronously cancel-able from another thread
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);

    // Set-up a timeout timer for the task simulation
    
    time_point start_time = std::chrono::system_clock::now();
    runtime_timer.expires_after(boost::asio::chrono::milliseconds(max_duration));
    runtime_timer.async_wait([&](const boost::system::error_code& error_code) {
        timeoutHandler(error_code, *this, start_time, pthread_self());
    });
}

void Task::tearDownTask(std::size_t simulation_score)
{
    runtime_timer.cancel();

    bool expected_value = false;
    bool is_simulation_finished = is_task_ended.compare_exchange_strong(expected_value, true);
    if (is_simulation_finished)
    {
        // Simulation finished successuly with NO timeout
        score = simulation_score;
        task_ended_();
    }
}
