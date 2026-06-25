#pragma once

#include "builtin_command.h"

class EchoCommand : public BuiltinCommand {
  private:
    virtual void Process(std::vector<Token> tokens) const override;

  public:
    EchoCommand(Output *output);
};
