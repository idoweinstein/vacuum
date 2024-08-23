#include "task.h"

#include <semaphore>
#include <vector>
#include <latch>

class TaskQueue
{
    // Queue Metadata
    std::latch& todo_tasks_counter;
    std::counting_semaphore& active_threads_semaphore;
    boost::asio::io_context timer_event_context;
    boost::asio::executor_work_guard<boost::asio::io_context> work_guard;
    std::jthread event_loop_thread;

    // Queue Contents
    std::vector<Task> tasks;

    void createTimer()
    {
        work_guard = boost::asio::make_work_guard(timer_event_context);

        event_loop_thread = std::jthread([&timer_event_context]() {
            timer_event_context.run();
        });

        active_threads_semaphore.acquire();
    }

public:
    TaskQueue(std::latch& todo_tasks_counter,
              std::counting_semaphore& active_threads_semaphore)
        : todo_tasks_counter(todo_tasks_counter),
          active_threads_semaphore(active_threads_semaphore),
    {
        createTimer();
    }

    void reserve(std::size_t reserved_size) { tasks.reserve(reserved_size); }

    void insertTask(std::string& algorithm_name,
                    std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
                    std::string& house_file_name,
                    bool is_logging)
    {
        tasks.emplace_back(
            *this,
            algorithm_name,
            std::move(algorithm_pointer),
            house_file_name,
            is_logging
        );
    }

    auto begin() const { return tasks.begin(); }

    const auto cbegin() const { return tasks.cbegin(); }

    auto end() const { return tasks.end(); }

    const auto cend() const { return tasks.cend(); }
};
