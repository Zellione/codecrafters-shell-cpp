#include "jobs.h"

unsigned int JobsRegistry::Add(BackgroundJob job)
{
    unsigned int job_number = m_jobs.empty() ? 1 : m_jobs.crbegin()->first + 1;
    m_jobs.emplace(job_number, job);

    return job_number;
}
BackgroundJob &JobsRegistry::Get(unsigned int job_number)
{
    return m_jobs.find(job_number)->second;
}

std::map<unsigned int, BackgroundJob> &JobsRegistry::GetAll() { return m_jobs; }
