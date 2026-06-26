#pragma once

#include "output/output.h"

struct CmdResult {
    std::string stdout_output;
    std::string stderr_output;
    int exit_code;
};

class ExecExternalCommand {
  private:
    TokenParser m_Parser;
    Output *m_Output;

  public:
    ExecExternalCommand(Output *output);
    [[nodiscard]] bool Exec(const std::string &commandline) const;

    static void ReadPipes(int stdout_fd, int stderr_fd, CmdResult &result);
};
