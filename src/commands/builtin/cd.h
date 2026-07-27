#pragma once

#include "command.h"

class CdCommand : public BuiltinCommand
{
  private:
    std::string &m_current_directory;

    [[nodiscard]] int Process(const Ast::Command &comm) override;

  public:
    CdCommand(std::string &current_directory);
};
