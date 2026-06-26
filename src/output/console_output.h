#pragma once

#include "output_type.h"

class ConsoleOutput : public OutputType {
  public:
    ConsoleOutput() = default;
    void Print(const std::vector<Token> &tokens,
               const std::string &out_buffer) const override;
    [[nodiscard]] bool IsApplicable(const std::vector<Token> &tokens,
                                    OutputTarget target) const override;
};
