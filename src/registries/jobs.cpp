#include "jobs.h"

unsigned int JobsRegistry::Add(unsigned int pid)
{
    unsigned int job_number = m_jobs.empty() ? 1 : m_jobs.crbegin()->first + 1;
    m_jobs.emplace(job_number, pid);

    return job_number;
}
unsigned int JobsRegistry::Get(unsigned int job_number) const
{
    return m_jobs.find(job_number)->second;
}

const std::map<unsigned int, unsigned int> &JobsRegistry::GetAll() const
{
    return m_jobs;
}
