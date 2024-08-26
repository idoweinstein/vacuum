#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_

struct Arguments
{
    std::string house_path;
    std::string algorithm_path;
    std::size_t num_threads;
    bool summary_only;
};

class InputHandler
{
    static bool parseArgument(const std::string& raw_argument, Main::arguments& arguments);

    static void searchDirectory(const std::string& directory_path_string,
                                const std::function<bool(const std::filesystem::directory_entry&)>& entry_filter,
                                const std::function<void(const std::filesystem::path&)>& found_operation);

public:
    InputHandler() = delete;

    static void openHouses(const std::string& house_directory_path, std::vector<std::string>& house_filenames);

    static void openAlgorithms(const std::string& algorithm_directory_path, std::vector<void*>& algorithm_handles);

    static void closeAlgorithms(std::vector<void*>& algorithm_handles);

    static bool parseCmdArguments(int argc, char* argv[], Arguments& arguments);
};

#endif /* INPUT_HANDLER_H_ */
