#pragma once

#include "builtin_command.h"
#include <functional>
#include <optional>

class BuiltinRegistry {
private:
  std::vector<BuiltinCommand *> m_commands;

public:
  const BuiltinCommand *FindCommand(std::string command) const;

  void RegisterCommand(BuiltinCommand *command);
};
