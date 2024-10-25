#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

#include "simulator/simulator.h"
#include "simulator/deserializer.h"

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

    /**
     * @brief Parses a single command line argument.
     * On success, sets the parsed argument into given arguments struct.
     * 
     * @param raw_argument The command line argument to be parsed.
     * @param arguments The struct to store the successfuly parsed argument into.
     * 
     * @throws std::invalid_argument On invalid argument given.
     * 
     * @return Whether or not to keep parsing (if parse was successful).
     */
    static bool parseArgument(const std::string& raw_argument, Arguments& arguments);

    /**
     * @brief A generic directory scanning utility function.
     * Used for searching all files applying a specified condition in a given directory,
     * and applying on these files a given operation.
     * 
     * @param directory_path_string The directory path to perform the scan at.
     * @param foundCriteria The filtering criteria - to filter relevant files in the scan.
     * @param onFound The found operation to be performed on each found file.
     */
    static void searchDirectory(const std::string& directory_path_string,
                                const std::function<bool(const std::filesystem::directory_entry&)>& foundCriteria,
                                const std::function<void(const std::filesystem::path&)>& onFound);

    /**
     * @brief A safe dlopen(), which validates both successful open and registration.
     * 
     * @param handle The handle which will store the successfully opened `.so`.
     * @param entry_path The path of the file to be dlopen()ed.
     */
    static bool safeDlOpen(void*& handle, const std::filesystem::path& file_path);

public:
    /**
    * @brief Deleted deault empty constructor.
    *
    * The default empty constructor is deleted since it's useless, as all the InputHandler member functions are `static`.
    */
    InputHandler() = delete;

    /**
     * @brief Find all `.house` files in a given directory.
     * 
     * @param house_directory_path The directory path to search the `.house` files at.
     * @param house_paths The vector to store found house file paths into.
     */
    static void findHouses(const std::string& house_directory_path, std::vector<std::filesystem::path>& house_paths);

    /**
     * @brief Reads house files from a given vector of house file paths.
     * 
     * @param house_paths The house file paths to be read.
     * @param house_files The read house files.
     */
    static void readHouses(const std::vector<std::filesystem::path>& house_paths, std::vector<HouseFile>& house_files);

    /**
     * @brief Find all `.so` files in a given directory and try dlopen()ing them as algorithms.
     * 
     * @param algorithm_directory_path The directory path to search the `.so` files at.
     * @param algorithm_handles The vector to store opened algorithm handles into.
     */
    static void openAlgorithms(const std::string& algorithm_directory_path, std::vector<std::shared_ptr<void>>& algorithm_handles);

    /**
     * @brief dlclose() all previously dlopen()ed algorithms `.so` files.
     * 
     * @param algorithm_handles The previously dlopen()ed algorithm files.
     */
    //static void closeAlgorithms(std::vector<void*>& algorithm_handles);

    /**
     * @brief Parses command line arguments.
     * 
     * @param argc Command line arguments count.
     * @param argv Command line arguments array.
     * @param arguments Struct to store parsed arguments into.
     * 
     * @return Whether or not to run the program (if arguments parsed successfuly).
     */
    static bool parseCmdArguments(int argc, char* argv[], Arguments& arguments);
};

#endif /* INPUT_HANDLER_H_ */
