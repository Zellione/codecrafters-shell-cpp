#pragma once

#include "command.h"

class EchoCommand : public BuiltinCommand
{
  private:
    [[nodiscard]] int Process(const std::vector<Token> &tokens) const override;

  public:
    EchoCommand(Output *output);
};
