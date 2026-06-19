#pragma once

#include "builtin_command.h"

class ExitCommand : public BuiltinCommand {
private:
  virtual void Process(std::vector<std::string> arguments) const override;

public:
  ExitCommand();
};
