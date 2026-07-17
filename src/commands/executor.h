#pragma once

#include "../parser/ast.h"
#include "../registries/jobs.h"
#include "builtin/registry.h"
#include "external/command.h"

class Executor
{
  private:
    Output &m_output;

    JobsRegistry m_jobs_registry;
    JobsRegistry m_background_jobs;
    BuiltinRegistry &m_builtin_registry;

    ExternalCommand &m_external_comm;

    int ExecSequence(Ast::Node **sequence, const std::vector<char *> &env_vars);
    int ExecPipeline(Ast::Node *pipeline, const std::vector<char *> &env_vars,
                     bool exec_last_in_place = false,
                     CmdResult *result = nullptr);
    int ExecCommand(const Ast::Command &command,
                    const std::vector<char *> &env_vars);

    int ExecBackground(Ast::Node *commandchain,
                       const std::vector<char *> &env_vars);
    bool HasBackgroundFlag(Ast::Node *nodes);

    static bool ReadPipes(int stdout_fd, int stderr_fd, fd_set &readfds,
                          std::string &out, std::string &err,
                          struct timeval &tv);

  public:
    Executor(Output &output, BuiltinRegistry &builtin_registry,
             ExternalCommand &ext_comm);

    int Exec(Ast::Node *sequence, const std::vector<char *> &env_vars,
             CmdResult *result = nullptr);

    int ReapBackgroundJobs();
    void ReapJobs();

    void PrintDoneBGJobs();

    [[nodiscard]] JobsRegistry &GetBGJobsRegistry();
};
