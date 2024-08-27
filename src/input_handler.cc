#include "input_handler.h"

#include "algorithm/AlgorithmRegistration.h"

#include "output_handler.h"

void InputHandler::searchDirectory(const std::string& directory_path_string,
                                   const std::function<bool(const std::filesystem::directory_entry&)>& entry_filter,
                                   const std::function<void(const std::filesystem::path&)>& found_operation)
{
    std::filesystem::path directory_path(directory_path_string);

    if (std::filesystem::exists(directory_path) && std::filesystem::is_directory(directory_path))
    {
        for (const auto& entry : std::filesystem::directory_iterator(directory_path))
        {
            if (entry_filter(entry))
            {
                continue;
            }

            found_operation(entry.path());
        }
    }

    else
    {
        throw std::runtime_error("Failed to open directory: " + directory_path_string);
    }
}

void InputHandler::openHouses(const std::string& house_directory_path, std::vector<std::string>& house_filenames)
{
    auto entry_filter = [](const std::filesystem::directory_entry& entry)
        {
            std::string entry_name = entry.path().filename().string();

            if (!entry.is_regular_file() ||
                entry_name.length() < house_format.size() ||
                entry_name.substr(entry_name.length() - house_format.size()) != house_format)
            {
                return true;
            }
            return false;
        };

    auto found_operation = [&house_filenames](const std::filesystem::path& entry_path)
        {
            house_filenames.emplace_back(entry_path);
        };

    searchDirectory(house_directory_path,
                    entry_filter,
                    found_operation);
}

void InputHandler::openAlgorithms(const std::string& algorithm_directory_path, std::vector<void*>& algorithm_handles)
{
    auto entry_filter = [](const std::filesystem::directory_entry& entry)
        {
            std::string entry_name = entry.path().filename().string();

            if (!entry.is_regular_file() ||
                entry_name.length() < algorithm_format.size() ||
                entry_name.substr(entry_name.length() - algorithm_format.size()) != algorithm_format)
            {
                return true;
            }

            return false;
        };

    auto found_operation = [&algorithm_handles](const std::filesystem::path& entry_path)
        {
            std::size_t pre_open_count = AlgorithmRegistrar::getAlgorithmRegistrar().count();
            void* handle = dlopen(entry_path.c_str(), RTLD_NOW);
            std::size_t post_open_count = AlgorithmRegistrar::getAlgorithmRegistrar().count();

            std::string algorithm_name = entry_path.stem().string(); 

            if (nullptr == handle)
            {
                OutputHandler::exportError(algorithm_name, "dlopen() failed!");
            }

            else if (post_open_count != pre_open_count + 1)
            {
                OutputHandler::exportError(algorithm_name, "dlopen() didn't increase registrar count!");
            }

            else
            {
                algorithm_handles.push_back(handle);
            }
        };

    searchDirectory(algorithm_directory_path,
                    entry_filter,
                    found_operation);
}

void InputHandler::closeAlgorithms(std::vector<void*>& algorithm_handles)
{
    for (void* handle : algorithm_handles)
    {
        dlclose(handle);
    }
}

bool InputHandler::parseArgument(const std::string& raw_argument, Arguments& arguments)
{
    if (raw_argument.starts_with("-house_path"))\
    {
        arguments.house_path = raw_argument.substr(raw_argument.find("=") + 1);
    }
    else if (raw_argument.starts_with("-algo_path"))
    {
        arguments.algorithm_path = raw_argument.substr(raw_argument.find("=") + 1);
    }
    else if (raw_argument.starts_with("-num_threads"))
    {
        arguments.num_threads = std::stoi(raw_argument.substr(raw_argument.find("=") + 1));
    }
    else if (raw_argument == "-summary_only")
    {
        arguments.summary_only = true;
    }
    else if (raw_argument.starts_with("-h") || raw_argument.starts_with("-help") || raw_argument.starts_with("--help"))
    {
        OutputHandler::printMessage("Usage: myrobot [-house_path=<path>] [-algo_path=<path>] [-num_threads=<num>] [-summary_only]");
        return false;
    }
    else
    {
        throw std::invalid_argument("Invalid argument: " + raw_argument);
    }

    return true;
}

bool InputHandler::parseCmdArguments(int argc, char* argv[], Arguments& arguments)
{
    for (int i = 1; i < argc; i++)
    {
        if (!parseArgument(argv[i], arguments))
        {
            return false;
        }
    }

    return true;
}
