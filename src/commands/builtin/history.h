#pragma once

#include "../../registries/history.h"
#include "command.h"

class HistoryCommand : public BuiltinCommand
{
  private:
    HistoryRegistry *m_registry;

    [[nodiscard]] int Process(const Ast::Command &comm) const override;

  public:
    HistoryCommand(HistoryRegistry *registry);
};
