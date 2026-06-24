#pragma once

#include "parser/token_parser.h"
#include <string>

class ExecExternalCommand {
  private:
    TokenParser m_Parser;

  public:
    ExecExternalCommand();
    bool Exec(std::string commandline) const;
};
