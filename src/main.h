#ifndef MAIN_H_
#define MAIN_H_

#include <utility>
#include <string>
#include <mutex>
#include <queue>

namespace Main
{
    typedef struct {
        std::string house_path;
        std::string algo_path;
        std::size_t num_threads;
        bool summary_only;
    } arguments;

    void runAll(const arguments& args);
}

#endif // MAIN_H_