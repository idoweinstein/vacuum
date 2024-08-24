#ifndef TASK_QUEUE_H_
#define TASK_QUEUE_H_

#include "task.h"
#include "common/abstract_algorithm.h"

#include <boost/asio.hpp>

#include <thread>
#include <semaphore>
#include <vector>
#include <latch>

class TaskQueue
{
    // Queue Metadata
    std::latch todo_tasks_counter;
    std::counting_semaphore<> active_threads_semaphore;
    boost::asio::io_context timer_event_context;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;
    std::jthread event_loop_thread;

    // Queue Contents
    std::vector<Task> tasks;

    void createTimer()
    {
        event_loop_thread = std::jthread([this]() {
            this->timer_event_context.run();
        });

        // TODO: remove this and check in the forum that sempahore # is the number of workers
        active_threads_semaphore.acquire();
    }

public:
    TaskQueue(size_t number_of_tasks,
              size_t number_of_threads)
        : todo_tasks_counter(number_of_tasks),
          active_threads_semaphore(number_of_threads),
          work_guard(boost::asio::make_work_guard(timer_event_context))
    {
        createTimer();
        tasks.reserve(number_of_tasks);
    }

    void insertTask(const std::string& algorithm_name,
                    std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
                    const std::string& house_file_name,
                    bool is_logging)
    {
        if (tasks.size() >= tasks.capacity())
        {
            throw std::logic_error("TaskQueue::insertTask() called after all tasks were inserted");
        }

        tasks.emplace_back(
            timer_event_context,
            [this]()
            {
                this->todo_tasks_counter.count_down();
                this->active_threads_semaphore.release();
            },
            algorithm_name,
            std::move(algorithm_pointer),
            house_file_name,
            is_logging
        );
    }

    void run()
    {
        if (tasks.size() <= tasks.capacity())
        {
            throw std::logic_error("TaskQueue::run() called before all tasks were inserted");
        }

        for (auto& task : tasks)
        {
            active_threads_semaphore.acquire();
            task.run();
        }

        todo_tasks_counter.wait();
    }

    auto begin() const { return tasks.begin(); }

    const auto cbegin() const { return tasks.cbegin(); }

    auto end() const { return tasks.end(); }

    const auto cend() const { return tasks.cend(); }
};

#endif // TASK_QUEUE_H_