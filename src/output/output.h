#pragma once

#include "../parser/token_parser.h"
#include "output_type.h"
#include <vector>

class Output {
  private:
    std::vector<OutputType *> m_outputs;

  public:
    Output();
    ~Output();

    void AddType(OutputType *output);

    void Put(const std::vector<Token> &tokens, const char *out_buffer);
};
