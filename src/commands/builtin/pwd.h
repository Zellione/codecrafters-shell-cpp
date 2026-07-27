#pragma once

#include "command.h"

class PwdCommand : public BuiltinCommand
{
  private:
    const std::string &m_current_directory;

    [[nodiscard]] int Process(const Ast::Command &comm) override;

  public:
    PwdCommand(const std::string &current_directory);
};
