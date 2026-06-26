#pragma once

#include "output/output.h"

class ExecExternalCommand {
  private:
    TokenParser m_Parser;
    Output *m_Output;

  public:
    ExecExternalCommand(Output *output);
    [[nodiscard]] bool Exec(const std::string &commandline) const;
};
