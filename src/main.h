#ifndef MAIN_H_
#define MAIN_H_

#include <string>

namespace Main
{
    struct Arguments {
        std::string house_path;
        std::string algo_path;
        std::size_t num_threads;
        bool summary_only;
    } ;

    void runAll(const Arguments& args);
}

#endif // MAIN_H_