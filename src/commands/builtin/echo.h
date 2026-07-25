#pragma once

#include "command.h"

class EchoCommand : public BuiltinCommand
{
  private:
    [[nodiscard]] int Process(const Ast::Command &comm) override;

  public:
    EchoCommand();
};
