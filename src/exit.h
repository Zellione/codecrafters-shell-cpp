#pragma once

#include "builtin_command.h"

class ExitCommand : public BuiltinCommand {
  private:
    virtual void Process(std::vector<Node> arguments) const override;

  public:
    ExitCommand();
};
