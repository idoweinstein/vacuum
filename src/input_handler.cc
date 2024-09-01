#include "input_handler.h"

#include "algorithm/AlgorithmRegistration.h"

#include "output_handler.h"

#include <exception>

void InputHandler::searchDirectory(const std::string& directory_path_string,
                                   const std::function<bool(const std::filesystem::directory_entry&)>& foundCriteria,
                                   const std::function<void(const std::filesystem::path&)>& onFound)
{
    std::filesystem::path directory_path(directory_path_string);

    if (std::filesystem::exists(directory_path) && std::filesystem::is_directory(directory_path))
    {
        for (const auto& entry : std::filesystem::directory_iterator(directory_path))
        {
            if (foundCriteria(entry))
            {
                onFound(entry.path());
            }
        }
    }

    else
    {
        throw std::runtime_error("Given path " + directory_path_string + " is not a valid directory!");
    }
}

void InputHandler::openHouses(const std::string& house_directory_path, std::vector<HouseFile>& house_files)
{
    auto isHouseFile = [](const std::filesystem::directory_entry& entry) -> bool
    {
        if (entry.is_regular_file() && kHouseExtension == entry.path().extension())
        {
            return true;
        }
        return false;
    };

    auto loadHouse = [&house_files](const std::filesystem::path& house_path)
    {
        house_files.emplace_back();

        try
        {
            Deserializer::readHouseFile(house_path, house_files.back());
        }

        catch (const std::exception& exception)
        {
            house_files.pop_back();
            std::string house_name = house_path.stem().string();
            OutputHandler::exportError(house_name, exception.what());
        }
    };

    searchDirectory(house_directory_path,
                    isHouseFile,
                    loadHouse);
}

bool InputHandler::safeDlOpen(void*& handle, const std::filesystem::path& entry_path)
{
    std::string algorithm_name = entry_path.stem().string(); 

    std::size_t pre_dlopen_count = AlgorithmRegistrar::getAlgorithmRegistrar().count();

    handle = dlopen(entry_path.c_str(), RTLD_NOW);
    if (nullptr == handle)
    {
        OutputHandler::exportError(algorithm_name, "dlopen() failed!");
        return false;
    }

    std::size_t post_dlopen_count = AlgorithmRegistrar::getAlgorithmRegistrar().count();

    if (1 + pre_dlopen_count != post_dlopen_count)
    {
        OutputHandler::exportError(algorithm_name, "dlopen() didn't increase registrar count!");
        return false;
    }

    return true;
}

void InputHandler::openAlgorithms(const std::string& algorithm_directory_path, std::vector<void*>& algorithm_handles)
{
    auto isAlgorithmFile = [](const std::filesystem::directory_entry& entry) -> bool
    {
        if (entry.is_regular_file() && kAlgorithmExtension == entry.path().extension())
        {
            return true;
        }
        return false;
    };

    auto loadAlgorithm = [&algorithm_handles](const std::filesystem::path& entry_path)
    {
        void* handle;
        if (safeDlOpen(handle, entry_path))
        {
            algorithm_handles.emplace_back(handle);
        }
    };

    searchDirectory(algorithm_directory_path,
                    isAlgorithmFile,
                    loadAlgorithm);
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
    if (raw_argument.starts_with("-house_path"))
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

    else if ("-summary_only" == raw_argument)
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
