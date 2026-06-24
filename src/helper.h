#pragma once

#include <filesystem>
#include <string>
#include <vector>

std::filesystem::path find_executable(std::string command);
std::vector<std::filesystem::path> get_valid_path_folders();
bool is_executable_file(const std::filesystem::path &path);
