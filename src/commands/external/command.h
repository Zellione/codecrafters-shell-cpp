#pragma once

#include "../../output/output.h"
#include "../../parser/ast.h"

struct CmdResult
{
    void Fill(const CmdResult &result)
    {
        stdout_output.append(result.stdout_output);
        stderr_output.append(result.stderr_output);
        exit_code = result.exit_code;
    }
    std::string stdout_output;
    std::string stderr_output;
    int exit_code;
};

class ExternalCommand
{
  private:
    Output *m_output;

    static void FillArgV(const Ast::Command &command,
                         std::vector<char *> &out_argv);

  public:
    ExternalCommand(Output *output);
    [[nodiscard]] static int Exec(const Ast::Command &command,
                           const std::vector<char *> &env_vars);

    void ExecCommand(const Ast::Command &command) const;

    static void ReadPipes(int stdout_fd, int stderr_fd, CmdResult &result);

    [[nodiscard]] static std::vector<std::string>
    SearchExecutable(const std::string &partial);
};
