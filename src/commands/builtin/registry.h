#pragma once

#include "command.h"

class BuiltinRegistry
{
  private:
    std::vector<BuiltinCommand *> m_commands;

  public:
    BuiltinRegistry() = default;
    ~BuiltinRegistry();

    [[nodiscard]] BuiltinCommand *FindCommand(const Ast::Command &comm) const;

    [[nodiscard]] BuiltinCommand *
    FindCommandInArguments(const Ast::Command &comm, size_t pos) const;

    [[nodiscard]] std::vector<std::string>
    AutoComplete(const std::string &partial) const;

    void RegisterCommand(BuiltinCommand *command);

    BuiltinCommand *GetCommandByName(const std::string &name);
};
