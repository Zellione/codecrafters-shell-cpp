#pragma once

#include "../../parser/ast.h"

class ErrorNotFound
{
  public:
    static void Raise(const Ast::Command &comm);
};
