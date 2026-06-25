#pragma once

#include "../parser/token_parser.h"
#include <vector>

class OutputType {
  public:
    OutputType() {}
    virtual void Print(const std::vector<Token> &tokens,
                       const char *out_buffer) const {};
    virtual bool IsApplicable(const std::vector<Token> &tokens) const {
        return false;
    };
};
