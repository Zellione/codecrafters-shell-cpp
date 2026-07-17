#pragma once

#include "output_type.h"

class ConsoleError : public OutputType
{
  public:
    ConsoleError() = default;
    void Print(const Ast::Command &comm,
               const std::string &out_buffer) const override;
    [[nodiscard]] bool IsApplicable(const Ast::Command &comm,
                                    OutputTarget target) const override;
};
