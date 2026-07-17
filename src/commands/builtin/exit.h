#pragma once

#include "command.h"

class ExitCommand : public BuiltinCommand
{
  private:
    [[nodiscard]] int Process(const Ast::Command &comm) const override;

  public:
    ExitCommand();
};
