#pragma once

#include "builtin_command.h"

class BuiltinRegistry {
  private:
    std::vector<BuiltinCommand *> m_commands;

  public:
    [[nodiscard]] const BuiltinCommand *
    FindCommand(const std::string &command) const;

    void RegisterCommand(BuiltinCommand *command);
};
