#pragma once

#include "../parser/ast.h"

enum class OutputTarget : std::uint8_t
{
    STDOUT,
    STDERR
};

class OutputType
{
  public:
    OutputType() = default;
    virtual void Print(const Ast::Command &command,
                       const std::string &out_buffer) const {};
    [[nodiscard]] virtual bool IsApplicable(const Ast::Command &command,
                                            OutputTarget target) const
    {
        return false;
    };
};
