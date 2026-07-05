#pragma once

#include "command.h"

class ExitCommand : public BuiltinCommand
{
  private:
    [[nodiscard]] int Process(const std::vector<Token> &tokens) const override;

  public:
    ExitCommand(Output *output);
};
