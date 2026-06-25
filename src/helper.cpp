#include "helper.h"
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <unistd.h>

std::filesystem::path find_executable(std::string command) {
    std::vector<std::filesystem::path> validPathFolders =
        get_valid_path_folders();

    for (const auto &path : validPathFolders) {

        std::filesystem::path filepath = path / command;
        std::stringstream cmd;
        if (filepath.string().find(' ') != std::string::npos ||
            filepath.string().find('\'') != std::string::npos ||
            filepath.string().find('\\') != std::string::npos) {
            cmd << std::quoted(filepath.string());
        } else {
            cmd << filepath.string();
        }

        if (std::filesystem::exists(filepath) && is_executable_file(filepath))
            return cmd.str();
    }

    return "";
}

std::vector<std::filesystem::path> get_valid_path_folders() {
    std::vector<std::filesystem::path> validPathFolders;
    const std::string path = std::string(std::getenv("PATH"));

    size_t pos = 0;
    while (pos < path.length()) {
        if (path[pos] == ';' || path[pos] == ':')
            pos++;

        size_t next = path.find_first_of(";:", pos);

        std::filesystem::path spath = "";
        if (next == std::string::npos) {
            spath = path.substr(pos);
        } else {
            size_t path_length = next - pos;
            spath = path.substr(pos, path_length);
        }

        if (std::filesystem::exists(spath)) {
            validPathFolders.emplace_back(spath);
        }

        pos = next;
    }

    return validPathFolders;
}

bool is_executable_file(const std::filesystem::path &filepath) {
    if (!std::filesystem::exists(filepath) ||
        !std::filesystem::is_regular_file(filepath)) {
        return false;
    }

    return access(filepath.c_str(), X_OK) == 0;
}

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) {
                return !std::isspace(c);
            }));
}

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char c) { return !std::isspace(c); })
                .base(),
            s.end());
}
