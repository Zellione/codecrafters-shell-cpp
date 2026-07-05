#pragma once

#include "../parser/token_parser.h"
#include "output_type.h"
#include <vector>

class Output
{
  private:
    std::vector<OutputType *> m_outputs;

  public:
    Output() = default;
    ~Output();

    void AddType(OutputType *output);

    void Put(const std::vector<Token> &tokens, const std::string &out_buffer,
             OutputTarget target) const;
};
