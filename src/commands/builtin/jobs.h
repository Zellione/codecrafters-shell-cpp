#pragma once

#include "command.h"

class JobsCommand : public BuiltinCommand
{
  private:
    void Process(const std::vector<Token> &tokens) const override;

  public:
    JobsCommand(Output *output);
};
