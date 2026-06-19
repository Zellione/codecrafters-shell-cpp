#include "builtin_registry.h"
#include "builtin_command.h"

const BuiltinCommand *BuiltinRegistry::FindCommand(std::string command) const {
  for (const auto builtin_com : m_commands) {
    if (builtin_com->IsCommand(command)) {
      return builtin_com;
    }
  }

  return nullptr;
}

void BuiltinRegistry::RegisterCommand(BuiltinCommand *command) {
  m_commands.push_back(command);
}
