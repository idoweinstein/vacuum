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
    // Queue Metadata
    std::size_t number_of_tasks;
    std::latch todo_tasks_counter;                      // An up-to-date counter of the number of tasks left to run.
    std::counting_semaphore<> active_threads_semaphore; // An up-to-date counter of the number of active WORKER (task) threads.

    std::jthread event_loop_thread;                     // A thread running the event loop (for task timeouts).
    boost::asio::io_context timer_event_context;        // Represents the event loop object.
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard;    // Prevents event loop from stopping with no active events.

    // Queue Contents
    std::list<Task> tasks;                            // The tasks in the queue to be executed.

    void createTimer();

public:
    TaskQueue(size_t number_of_tasks, size_t number_of_threads);

    void insertTask(const std::string& algorithm_name,
                    std::unique_ptr<AbstractAlgorithm>&& algorithm_pointer,
                    const std::string& house_name);

    void run();

    auto begin() { return tasks.begin(); }

    auto end() { return tasks.end(); }
};

#endif // TASK_QUEUE_H_