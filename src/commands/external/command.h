#pragma once

#include "../../output/output.h"
#include "../../registries/jobs.h"

struct CmdResult
{
    void Fill(const CmdResult &result)
    {
        stdout_output = result.stdout_output;
        stderr_output = result.stderr_output;
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
    JobsRegistry *m_registry;

    [[nodiscard]] static bool
    HasBackgroundFlag(const std::vector<Token> &tokens);

  public:
    ExternalCommand(Output *output, JobsRegistry *registry);
    [[nodiscard]] bool Exec(const std::vector<Token> &tokens,
                            const std::vector<char *> &env_vars,
                            CmdResult *result_out = nullptr) const;

    static void ReadPipes(int stdout_fd, int stderr_fd, CmdResult &result);

    [[nodiscard]] static std::vector<std::string>
    SearchExecutable(const std::string &partial);
};
