#include "helper.h"
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <ranges>
#include <sstream>
#include <unistd.h>

using std::string;

string get_path() {
    char *buffer = std::getenv("PATH");
    if (buffer != nullptr) {
        return string{buffer};
    }

    return "";
}

std::filesystem::path find_executable(const std::string &command) {
    std::vector<std::filesystem::path> validPathFolders =
        get_valid_path_folders();

    for (const auto &path : validPathFolders) {

        std::filesystem::path filepath = path / command;
        std::stringstream cmd;
        if (filepath.string().contains(' ') ||
            filepath.string().contains('\'') ||
            filepath.string().contains('\\')) {
            cmd << std::quoted(filepath.string());
        } else {
            cmd << filepath.string();
        }

        if (std::filesystem::exists(filepath) && is_executable_file(filepath)) {
            return cmd.str();
        }
    }

    return "";
}

std::vector<std::filesystem::path> get_valid_path_folders() {
    std::vector<std::filesystem::path> validPathFolders;
    const std::string path = get_path();

    size_t pos = 0;
    while (pos < path.length()) {
        if (path[pos] == ';' || path[pos] == ':') {
            pos++;
        }

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
    s.erase(s.begin(), std::ranges::find_if(s, [](unsigned char c) {
                return !std::isspace(c);
            }));
}

void rtrim(std::string &s) {
    s.erase(
        std::ranges::find_if(std::views::reverse(s),
                             [](unsigned char c) { return !std::isspace(c); })
            .base(),
        s.end());
}
