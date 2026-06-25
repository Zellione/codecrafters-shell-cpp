#pragma once

#include "builtin_command.h"

class ExitCommand : public BuiltinCommand {
  private:
    virtual void Process(std::vector<Token> tokens) const override;

  public:
    ExitCommand(Output *output);
};
