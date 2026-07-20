#pragma once

#include <string>

#include "../../parser/ast.h"

class BuiltinCommand
{
  private:
    std::string m_name;

    [[nodiscard]] virtual int Process(const Ast::Command &command) const;

  public:
    BuiltinCommand(std::string name);

    [[nodiscard]] virtual bool IsCommand(const std::string &command) const;
    [[nodiscard]] virtual bool NameStartWith(const std::string &partial) const;

    [[nodiscard]] const std::string &GetName() const;

    [[nodiscard]] virtual int Execute(const Ast::Command &command) const;
};
