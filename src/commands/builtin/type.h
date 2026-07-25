#pragma once

#include "command.h"
#include "registry.h"

class TypeCommand : public BuiltinCommand
{
  private:
    [[nodiscard]] int Process(const Ast::Command &comm) override;

    const BuiltinRegistry *m_registry;

  public:
    TypeCommand(const BuiltinRegistry *registry);
};
