#ifndef TASK_QUEUE_H_
#define TASK_QUEUE_H_

#include "task.h"
#include "common/abstract_algorithm.h"

#include <boost/asio.hpp>

#include <semaphore>
#include <thread>
#include <latch>
#include <list>

class TaskQueue
{
    // Queue Synchronization Metadata
    std::size_t num_runnable_tasks;
    std::latch todo_tasks_counter;
    std::counting_semaphore<> active_threads_semaphore; // An up-to-date counter of the number of active WORKER (task) threads.

    // Queue Timing Utilities
    std::jthread timer_thread;                          // A thread running the event loop (for task timeouts).
    boost::asio::io_context timer_context;              // Represents the event loop object.
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;    // Prevents event loop from stopping with no active events.

    // Queue Contents
    std::list<Task> tasks;                            // The tasks in the queue to be executed.

    void createTimer();

    void checkTaskInsertion();

public:
    TaskQueue(std::size_t number_of_tasks, std::size_t number_of_threads);

    void insertTask(const std::string& algorithm_name,
                    std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
                    const std::filesystem::path& house_path);

    void run();

    auto begin() { return tasks.begin(); }

    auto end() { return tasks.end(); }
};

#endif // TASK_QUEUE_H_