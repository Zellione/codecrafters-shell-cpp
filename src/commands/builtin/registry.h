#pragma once

#include "command.h"

class BuiltinRegistry
{
  private:
    std::vector<BuiltinCommand *> m_commands;

  public:
    BuiltinRegistry() = default;
    ~BuiltinRegistry();

    [[nodiscard]] const BuiltinCommand *
    FindCommand(const std::vector<Token> &tokens) const;

    [[nodiscard]] const BuiltinCommand *
    FindCommandInArguments(const std::vector<Token> &tokens, size_t pos) const;

    [[nodiscard]] std::vector<std::string>
    AutoComplete(const std::string &partial) const;

    void RegisterCommand(BuiltinCommand *command);
};
