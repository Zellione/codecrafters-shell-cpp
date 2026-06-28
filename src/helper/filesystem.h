#pragma once

#include <filesystem>
#include <string>
#include <vector>

std::vector<std::string>
get_files_from_dir(const std::filesystem::path &path,
                   const std::string &relative_path = "");

std::vector<std::string>
get_subdirs_from_dir(const std::filesystem::path &path,
                     const std::string &relative_path = "");
