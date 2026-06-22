#pragma once

#include "builtin_command.h"

class EchoCommand : public BuiltinCommand {
  private:
    virtual void Process(std::vector<std::string> arguments) const override;

  public:
    EchoCommand();
};
