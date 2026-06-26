#pragma once

#include <filesystem>
#include <string>
#include <vector>

std::filesystem::path find_executable(const std::string& command);
std::vector<std::filesystem::path> get_valid_path_folders();
bool is_executable_file(const std::filesystem::path &path);

void ltrim(std::string &s);

void rtrim(std::string &s);
