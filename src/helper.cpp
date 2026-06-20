#include "helper.h"
#include <filesystem>
#include <unistd.h>

std::filesystem::path find_executable(std::string command) {
    std::vector<std::filesystem::path> validPathFolders =
        get_valid_path_folders();

    for (const auto &path : validPathFolders) {

        std::filesystem::path filepath = path / command;

        if (std::filesystem::exists(filepath) && is_executable_file(filepath))
            return filepath;
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
    std::filesystem::path path = std::filesystem::path(filepath);
    if (!std::filesystem::exists(path) ||
        !std::filesystem::is_regular_file(path)) {
        return false;
    }

    return access(path.c_str(), X_OK) == 0;
}

std::vector<std::string> get_command_and_args(std::string commandline) {
    std::vector<std::string> comm_and_args;

    size_t pos = 0;
    while (pos < commandline.length()) {
        if (commandline[pos] == ' ')
            pos++;

        size_t next = commandline.find(' ', pos);

        if (next == std::string::npos) {
            comm_and_args.emplace_back(commandline.substr(pos));
            break;
        }

        size_t arg_length = next - pos;
        comm_and_args.emplace_back(commandline.substr(pos, arg_length));

        pos = next;
    }

    return comm_and_args;
}
