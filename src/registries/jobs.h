#pragma once

#include <map>
class JobsRegistry
{
  private:
    std::map<unsigned int, unsigned int> m_jobs;

  public:
    JobsRegistry() = default;

    unsigned int Add(unsigned int pid);
    [[nodiscard]] unsigned int Get(unsigned int job_number) const;
    [[nodiscard]] const std::map<unsigned int, unsigned int> &GetAll() const;
};
