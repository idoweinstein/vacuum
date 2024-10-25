#include "task_queue.h"

void TaskQueue::createTimer()
{
    timer_thread = std::jthread([this]() {
        this->timer_context.run();
    });

    /**
     * NOTE: The event loop thread is not part of the thread pool.
     * Therefore, it is not counted in the number of active threads (counted in `active_threads_sempahore`).
     * See: https://moodle.tau.ac.il/mod/forum/discuss.php?d=106205
     */
}

TaskQueue::TaskQueue(std::size_t number_of_tasks, std::size_t number_of_threads)
    : num_tasks(number_of_tasks),
      todo_tasks_counter(number_of_tasks),
      active_threads_semaphore(number_of_threads),
      work_guard(boost::asio::make_work_guard(timer_context))
{
    createTimer();
}

void TaskQueue::insertTask(const std::string& algorithm_name,
                           std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
                           const HouseFile& house_file,
                           std::shared_ptr<void>& algorithm_handle)
{
    if (tasks.size() >= num_tasks)
    {
        throw std::out_of_range("TaskQueue::insertTask() was called after all tasks were inserted.");
    }

    static auto taskTearDown = [this]()
    {
        this->todo_tasks_counter.count_down();
        this->active_threads_semaphore.release();
    };

    tasks.emplace_back(
        algorithm_name,
        algorithm_handle,
        std::move(algorithm_pointer),
        house_file,
        taskTearDown,
        timer_context
    );
}

void TaskQueue::run()
{
    if (tasks.size() < num_tasks)
    {
        throw std::logic_error("TaskQueue::run() was called before all tasks were inserted.");
    }

    for (auto& task : tasks)
    {
        // Semaphore used to check we can spawn another task without exceeding the maximal threads limit.
        active_threads_semaphore.acquire();
        task.run();
    }

    // Wait for all tasks to finish running (gracefully or due to a timeout).
    todo_tasks_counter.wait();

    std::cout << "All tasks finished running" << std::endl;
    timer_context.stop();
    std::cout << "Timer context stopped" << std::endl;
}
