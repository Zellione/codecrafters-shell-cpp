#pragma once

#include <map>
#include <string>
#include <sys/types.h>

#include "../output/output.h"

enum class BackgroundJobStatus : u_int8_t
{
    RUNNING,
    DONE
};

struct BackgroundJob
{
    pid_t pid;
    int read_fd_out;
    int read_fd_err;
    BackgroundJobStatus status;
    std::string commandline;
};

class JobsRegistry
{
  private:
    std::map<unsigned int, BackgroundJob> m_jobs;

  public:
    JobsRegistry() = default;

    unsigned int Add(BackgroundJob job);
    [[nodiscard]] BackgroundJob &Get(unsigned int job_number);
    [[nodiscard]] std::map<unsigned int, BackgroundJob> &GetAll();

    void Cleanup();
    void PrintDone(const Output &output);
};
