#include "task.h"

#include <sched.h>
#include <stop_token>

#include "output_handler.h"

void Task::setIdlePriority(pthread_t& thread_handler)
{
    sched_param parameters;
    parameters.sched_priority = sched_get_priority_min(SCHED_IDLE);
    pthread_setschedparam(thread_handler, SCHED_IDLE, &parameters);
}

void Task::timeoutHandler(const boost::system::error_code& error_code,
                          Task& task,
                          pthread_t thread_handler,
                          boost::asio::steady_timer& runtime_timer)
{
    // Make sure timer was not cancelled.
    if (!error_code)
    {
        bool expected_value = false;
        bool is_simulation_timeout = task.is_task_ended.compare_exchange_strong(expected_value, true);
        if (is_simulation_timeout)
        {
            task.score = task.timeout_score;
            task.onTeardown();
            setIdlePriority(thread_handler);
            task.stop();
            runtime_timer.cancel();
        }
    }
}

Task::Task(const std::string& algorithm_name,
           std::shared_ptr<void>& algorithm_handle,
           std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
           const HouseFile& house_file,
           std::function<void()> onTeardown,
           boost::asio::io_context& timer_context)
    : algorithm_name(algorithm_name),
      algorithm_handle(algorithm_handle),
      algorithm_pointer(std::move(algorithm_pointer)),
      house_name(house_file.name),
      house_file(house_file),
      is_task_ended(false),
      onTeardown(onTeardown),
      timer_context(timer_context)
{
}

void Task::setUpTask(boost::asio::steady_timer& runtime_timer)
{
    // Set-up a timeout timer for the task simulation
    runtime_timer.expires_after(boost::asio::chrono::milliseconds(max_duration));
    auto current_thread = pthread_self();
    runtime_timer.async_wait([this, current_thread, &runtime_timer](const boost::system::error_code& error_code) {
        timeoutHandler(error_code, *this, current_thread, runtime_timer);
    });
}

void Task::tearDownTask(Simulator& simulator, std::optional<std::size_t> simulation_score, boost::asio::steady_timer& runtime_timer)
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

void Task::simulatePair(std::stop_token stop_token)
{
    std::shared_ptr<void> algorithm_handle_copy = algorithm_handle;
    std::unique_ptr<AbstractAlgorithm> algorithm_pointer_copy = std::move(algorithm_pointer);
    Simulator simulator(house_file);
    simulator.setAlgorithm(*(algorithm_pointer_copy));
    max_duration = simulator.getMaxSteps();
    timeout_score = simulator.getTimeoutScore();

    boost::asio::steady_timer runtime_timer(timer_context);
    setUpTask(runtime_timer);

    std::optional<std::size_t> simulation_score;

    try
    {
        simulation_score = simulator.run(stop_token);
        statistics = simulator.getSimulationStatistics();
    }

    catch(const std::exception& exception)
    {
        setAlgorithmError(exception.what());
    }

    tearDownTask(simulator, simulation_score, runtime_timer);
}
