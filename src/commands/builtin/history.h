#pragma once

#include "command.h"

class HistoryCommand : public BuiltinCommand
{
  private:
    [[nodiscard]] int Process(const Ast::Command &comm) const override;

  public:
    HistoryCommand();
};
