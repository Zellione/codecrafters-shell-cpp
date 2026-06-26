#pragma once

#include "builtin_command.h"
#include "builtin_registry.h"

class TypeCommand : public BuiltinCommand {
  private:
    void Process(const std::vector<Token> &tokens) const override;

    const BuiltinRegistry *m_registry;

  public:
    TypeCommand(const BuiltinRegistry *registry, Output *output);
};
