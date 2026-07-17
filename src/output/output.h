#pragma once

#include "../parser/ast.h"
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

    void Put(const Ast::Command &comm, const std::string &out_buffer,
             OutputTarget target) const;
};
