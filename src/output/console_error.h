#pragma once

#include "output_type.h"

class ConsoleError : public OutputType
{
  public:
    ConsoleError() = default;
    void Print(const std::vector<Token> &tokens,
               const std::string &out_buffer) const override;
    [[nodiscard]] bool IsApplicable(const std::vector<Token> &tokens,
                                    OutputTarget target) const override;
};
