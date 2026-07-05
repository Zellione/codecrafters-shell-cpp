#pragma once

#include <map>
#include <string>
#include <sys/types.h>

struct BackgroundJob
{
    pid_t pid;
    int read_fd_out;
    int read_fd_err;
    std::string command_name;
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
};
