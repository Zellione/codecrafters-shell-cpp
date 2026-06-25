#pragma once

#include "output/output.h"
#include "parser/token_parser.h"

class ExecExternalCommand {
  private:
    TokenParser m_Parser;
    Output *m_Output;

  public:
    ExecExternalCommand(Output *output);
    bool Exec(const std::string &commandline) const;
};
