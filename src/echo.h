#pragma once

#include "builtin_command.h"

class EchoCommand : public BuiltinCommand {
  private:
    void Process(const std::vector<Token> &tokens) const override;

  public:
    EchoCommand(Output *output);
};
