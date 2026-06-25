#pragma once

#include "output_type.h"

class ConsoleOutput : public OutputType {
  public:
    ConsoleOutput() {}
    virtual void Print(const std::vector<Token> &tokens,
                       const char *out_buffer) const override;
    virtual bool IsApplicable(const std::vector<Token> &tokens) const override;
};
