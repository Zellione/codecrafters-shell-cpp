#include "jobs.h"
#include "command.h"
#include <format>
#include <sstream>
#include <string>

using Ast::Command;

JobsCommand::JobsCommand(JobsRegistry &registry)
    : BuiltinCommand("jobs"), m_registry(registry)
{
}

int JobsCommand::Process(const Command &comm) const
{
    std::map<unsigned int, BackgroundJob> jobs = m_registry.GetAll();

    if (jobs.empty())
    {
        return 0;
    }

    unsigned int current_number = std::prev(jobs.end())->first;
    unsigned int previous_number =
        jobs.size() > 1 ? std::prev(std::prev(jobs.end()))->first : 0;
    for (auto &job : jobs)
    {
        char job_marker = ' ';
        if (job.first == current_number)
        {
            job_marker = '+';
        }
        if (job.first == previous_number)
        {
            job_marker = '-';
        }

        // Status column (Running) is padded to 24 characters (7 characters + 17
        // characters space)
        std::cout << std::format("[{}]{}  {}{}\n", job.first, job_marker,
                                 GenerateProcessStatus(job.second.status),
                                 job.second.commandline);
    }

    m_registry.Cleanup();
    return 0;
}

std::string JobsCommand::GenerateProcessStatus(BackgroundJobStatus status)
{
    std::stringstream ss;
    std::string status_string;

    switch (status)
    {
    case BackgroundJobStatus::RUNNING:
        status_string = "Running";
        break;
    case BackgroundJobStatus::DONE:
        status_string = "Done";
        break;
    }
    ss << status_string;
    for (size_t i = 0; i < 24 - status_string.length(); i++)
    {
        ss << " ";
    }

    return ss.str();
}
