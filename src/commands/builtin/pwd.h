#pragma once

#include "command.h"

class PwdCommand : public BuiltinCommand
{
  private:
    [[nodiscard]] int Process(const Ast::Command &comm) override;

  public:
    PwdCommand();
};
