#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include "simulator/simulator.h"

#include <filesystem>
#include <functional>
#include <cstddef>
#include <dlfcn.h>
#include <vector>
#include <string>

using namespace std::string_literals;

struct Arguments
{
    std::string house_path;
    std::string algorithm_path;
    std::size_t num_threads;
    bool summary_only;
};

class InputHandler
{
    inline static const std::string kAlgorithmExtension = ".so"s;
    inline static const std::string kHouseExtension = ".house"s;

    static bool parseArgument(const std::string& raw_argument, Arguments& arguments);

    static void searchDirectory(const std::string& directory_path_string,
                                const std::function<bool(const std::filesystem::directory_entry&)>& foundCriteria,
                                const std::function<void(const std::filesystem::path&)>& onFound);

    static bool safeDlOpen(void*& handle, const std::filesystem::path& entry_path);

public:
    InputHandler() = delete;

    static void openHouses(const std::string& house_directory_path, std::vector<std::filesystem::path>& house_paths);

    static void openAlgorithms(const std::string& algorithm_directory_path, std::vector<void*>& algorithm_handles);

    static void closeAlgorithms(std::vector<void*>& algorithm_handles);

    static bool parseCmdArguments(int argc, char* argv[], Arguments& arguments);
};

#endif /* INPUT_HANDLER_H_ */
