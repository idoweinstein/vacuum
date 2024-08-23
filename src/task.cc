#include "task.h"

void Task::timeoutHandler(boost::system::error_code& error_code,
                          Task& task,
                          time_point& start_time,
                          pthread_t thread_handler)
{
    // Make sure timer was not cancelled.
    if (boost::asio::error::operation_aborted != error_code) // TODO: Make sure it's correct
    {
        bool expected_value = false;
        bool is_simulation_timeout = task.is_task_ended.compare_exchange_strong(expected_value, true);
        if (is_simulation_timeout)
        {
            pthread_cancel(thread_handler);
            task.task_queue.todo_tasks_counter.count_down();
            task_queue.active_threads_semaphore.release();
        }
    }
}

Task::Task(TaskQueue& task_queue,
           std::string& algorithm_name,
           std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
           std::string& house_file_name,
           bool is_logging)
            : task_queue(task_queue),
              algorithm_name(algorithm_name),
              algorithm_pointer(std::move(algorithm_pointer)),
              house_file_name(house_file_name),
              is_logging(is_logging),
              is_task_ended(false)
{
    simulator.readHouseFile(house_name, is_logging);
    simulator.setAlgorithm(*algorithm_pointer);

    max_duration = 1ms * simulator.getMaxSteps();
}

void Task::setUpTask()
{
    // Make this thread asynchronously cancel-able from another thread
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);

    // Set-up a timeout timer for the task simulation
    boost::asio::steady_timer runtime_timer(task_queue.timer_event_context, std::chrono:milliseconds(max_duration));
    time_point start_time = std::chrono::system_clock::now();
    runtime_timer.async_wait([&](const boost::system::error_code& error_code) {
        timeoutHandler(error_code, *this, start_time, pthread_self())
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
        task_queue.todo_tasks_counter.count_down();
        task_queue.active_threads_semaphore.release();
    }
}
