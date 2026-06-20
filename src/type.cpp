#include "type.h"
#include "builtin_command.h"

#include <filesystem>
#include <iostream>
#include <unistd.h>

TypeCommand::TypeCommand(const BuiltinRegistry *registry)
    : BuiltinCommand("type", "type is a shell builtin"), m_registry(registry) {}

void TypeCommand::Process(std::vector<std::string> arguments) const {
  std::string type_check = arguments.size() > 0 ? arguments[0] : "";

  const BuiltinCommand *command = m_registry->FindCommand(type_check);

  if (command != nullptr) {
    std::cout << command->GetDescription() << std::endl;
    return;
  }

  std::string executable = FindExecutable(type_check);
  if (executable != "") {
    std::cout << type_check << " is " << executable << std::endl;
    return;
  }

  std::cout << type_check << ": not found" << std::endl;
}

std::string TypeCommand::FindExecutable(std::string command) const {
  std::vector<std::string> validPathFolders = GetValidPathFolders();

  for (const auto &path : validPathFolders) {
    std::string filepath = std::format("{}/{}", path, command);
    if (std::filesystem::exists(filepath) && IsExecutableFile(filepath))
      return filepath;
  }

  return "";
}

std::vector<std::string> TypeCommand::GetValidPathFolders() const {
  std::vector<std::string> validPathFolders;
  const std::string path = std::string(std::getenv("PATH"));

  size_t pos = 0;
  while (pos < path.length()) {
    if (path[pos] == ';' || path[pos] == ':')
      pos++;

    size_t next = path.find_first_of(";:", pos);

    std::string spath = "";
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

bool TypeCommand::IsExecutableFile(const std::string &filepath) const {

  std::filesystem::path path = std::filesystem::path(filepath);
  if (!std::filesystem::exists(path) ||
      !std::filesystem::is_regular_file(path)) {
    return false;
  }

  return access(path.c_str(), X_OK) == 0;
}
