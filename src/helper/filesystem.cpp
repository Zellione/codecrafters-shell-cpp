#include "filesystem.h"

std::vector<std::string> get_files_from_dir(const std::filesystem::path &path)
{
    std::vector<std::string> files;
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        std::string file = entry.path().filename().string();
        if (std::filesystem::is_regular_file(entry.status()) &&
            std::ranges::find(files, file) == files.end())
        {
            files.push_back(file);
        }
    }

    return files;
}

std::vector<std::string> get_subdirs_from_dir(const std::filesystem::path &path)
{
    std::vector<std::string> subdirs;
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        std::string file = entry.path().filename().string();
        if (!std::filesystem::is_regular_file(entry.status()) &&
            std::ranges::find(subdirs, file) == subdirs.end())
        {
            subdirs.push_back(std::format("{}/", file));
        }
    }

    return subdirs;
}
