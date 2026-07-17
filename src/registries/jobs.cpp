#include "jobs.h"
#include "../commands/builtin/jobs.h"

#include <format>
#include <ranges>

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

void JobsRegistry::Cleanup()
{
    for (auto it = m_jobs.begin(); it != m_jobs.end();)
    {
        if (it->second.status == BackgroundJobStatus::DONE)
        {
            it = m_jobs.erase(it);
            continue;
        }
        it++;
    }
}

void JobsRegistry::PrintDone(const Output &output)
{
    auto jobs = m_jobs | std::views::filter([](const auto &pair) {
                    return pair.second.status == BackgroundJobStatus::DONE;
                }) |
                std::ranges::to<std::map>();

    if (jobs.empty())
    {
        return;
    }

    unsigned int current_number = std::prev(jobs.end())->first;
    unsigned int previous_number =
        jobs.size() > 1 ? std::prev(std::prev(jobs.end()))->first : 0;
    for (const auto &[key, value] : jobs)
    {
        if (value.status != BackgroundJobStatus::DONE)
        {
            continue;
        }

        char job_marker = ' ';
        if (key == current_number)
        {
            job_marker = '+';
        }
        if (key == previous_number)
        {
            job_marker = '-';
        }

        output.Put({},
                   std::format("[{}]{}  {}{}\n", key, job_marker,
                               JobsCommand::GenerateProcessStatus(value.status),
                               value.commandline),
                   OutputTarget::STDOUT);
    }

    Cleanup();
}
