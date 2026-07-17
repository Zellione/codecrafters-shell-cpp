#pragma once

#include "output_type.h"

class ConsoleOutput : public OutputType
{
  public:
    ConsoleOutput() = default;
    void Print(const Ast::Command &comm,
               const std::string &out_buffer) const override;
    [[nodiscard]] bool IsApplicable(const Ast::Command &comm,
                                    OutputTarget target) const override;
};
