#pragma once

#include "../../output/output.h"

struct CmdResult
{
    std::string stdout_output;
    std::string stderr_output;
    int exit_code;
};

class ExternalCommand
{
  private:
    Output *m_Output;

  public:
    ExternalCommand(Output *output);
    [[nodiscard]] bool Exec(const std::vector<Token> &tokens) const;

    static void ReadPipes(int stdout_fd, int stderr_fd, CmdResult &result);

    [[nodiscard]] static std::vector<std::string>
    SearchExecutable(const std::string &partial);
};
