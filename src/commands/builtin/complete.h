#pragma once

#include "command.h"

class CompleteCommand : public BuiltinCommand
{
  private:
    void Process(const std::vector<Token> &tokens) const override;

  public:
    CompleteCommand(Output *output);
};
