#pragma once

#include "../registries/variables.h"
#include "ast.h"

class Expander
{
  private:
    Ast::Node *m_start_node;
    const VariableRegistry &m_variables;

    void WalkTree(Ast::Node *node);
    void ReplaceVars(Ast::Command &comm);

  public:
    Expander(Ast::Node *start_node, const VariableRegistry &variables);

    void Expand();
};
