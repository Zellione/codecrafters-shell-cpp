#include "filesystem.h"

std::vector<std::string> get_files_from_dir(const std::filesystem::path &path,
                                            const std::string &relative_path)
{
    std::vector<std::string> files;
    for (const auto &entry : std::filesystem::directory_iterator(path))
    {
        std::string file = entry.path().filename().string();
        if (std::filesystem::is_regular_file(entry.status()) &&
            std::ranges::find(files, file) == files.end())
        {
            if (!relative_path.empty())
            {
                file = std::format("{}/{}", relative_path, file);
            }
            files.push_back(file);
        }
    }

    return files;
}
