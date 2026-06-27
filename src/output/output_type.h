#pragma once

#include "../parser/token_parser.h"
#include <vector>

enum class OutputTarget : std::uint8_t
{
    NONE,
    STDOUT,
    STDERR
};

class OutputType
{
  public:
    OutputType() = default;
    virtual void Print(const std::vector<Token> &tokens,
                       const std::string &out_buffer) const {};
    [[nodiscard]] virtual bool IsApplicable(const std::vector<Token> &tokens,
                                            OutputTarget target) const
    {
        return false;
    };
};
