#pragma once

#include "builtin_command.h"

class ExitCommand : public BuiltinCommand {
  private:
    void Process(const std::vector<Token> &tokens) const override;

  public:
    ExitCommand(Output *output);
};
