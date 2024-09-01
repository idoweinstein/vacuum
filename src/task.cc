#include "task.h"

#include <sched.h>

#include "output_handler.h"

void Task::setIdlePriority(pthread_t& thread_handler)
{
    sched_param parameters;
    parameters.sched_priority = sched_get_priority_min(SCHED_IDLE);
    pthread_setschedparam(thread_handler, SCHED_IDLE, &parameters);
}

void Task::timeoutHandler(const boost::system::error_code& error_code,
                          Task& task,
                          pthread_t thread_handler)
{
    // Make sure timer was not cancelled.
    if (!error_code)
    {
        bool expected_value = false;
        bool is_simulation_timeout = task.is_task_ended.compare_exchange_strong(expected_value, true);
        if (is_simulation_timeout)
        {
            task.score = task.simulator.getTimeoutScore();
            task.onTeardown();
            setIdlePriority(thread_handler);
        }
    }
}

Task::Task(const std::string& algorithm_name,
           std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
           const std::filesystem::path& house_path,
           std::function<void()> onTeardown,
           boost::asio::io_context& timer_context)
    : is_runnable(true),
      algorithm_name(algorithm_name),
      algorithm_pointer(std::move(algorithm_pointer)),
      house_name(house_path.stem().string()),
      is_task_ended(false),
      onTeardown(onTeardown),
      timer_context(timer_context)
{
    try
    {
        simulator.readHouseFile(house_path);
        simulator.setAlgorithm(*(this->algorithm_pointer));
        max_duration = simulator.getMaxSteps();
    }

    catch(const std::exception& exception)
    {
        is_runnable = false;
        OutputHandler::exportError(house_name, exception.what());
    }
}

void Task::setUpTask(boost::asio::steady_timer& runtime_timer)
{
    // Set-up a timeout timer for the task simulation
    runtime_timer.expires_after(boost::asio::chrono::milliseconds(max_duration));
    auto current_thread = pthread_self();
    runtime_timer.async_wait([this, current_thread](const boost::system::error_code& error_code) {
        timeoutHandler(error_code, *this, current_thread);
    });
}

void Task::tearDownTask(std::optional<std::size_t> simulation_score, boost::asio::steady_timer& runtime_timer)
{
    runtime_timer.cancel();

    bool expected_value = false;
    bool is_finished_gracefully = is_task_ended.compare_exchange_strong(expected_value, true);
    if (is_finished_gracefully)
    {
        if (simulation_score.has_value())
        {
            // Simulation finished successfully (with NO timeout)
            score = simulation_score.value();
        }

        else
        {
            // Simulation threw an exception (with NO timeout)
            score = simulator.getTimeoutScore();
        }

        onTeardown();
    }
}

void Task::simulatePair()
{
    boost::asio::steady_timer runtime_timer(timer_context);
    setUpTask(runtime_timer);

    std::optional<std::size_t> simulation_score;

    try
    {
        simulation_score = simulator.run();
    }

    catch(const std::exception& exception)
    {
        setAlgorithmError(exception.what());
    }

    tearDownTask(simulation_score, runtime_timer);
}
