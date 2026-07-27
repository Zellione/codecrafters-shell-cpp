#pragma once

#include "../../registries/variables.h"
#include "command.h"

class DeclareCommand : public BuiltinCommand
{
  private:
    VariableRegistry &m_variables;

    [[nodiscard]] int Process(const Ast::Command &comm) override;

    static bool IsValidVariableName(const std::string &name);

  public:
    DeclareCommand(VariableRegistry &registry);
};
