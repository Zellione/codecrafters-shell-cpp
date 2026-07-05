#include "jobs.h"
#include "command.h"
#include <format>

JobsCommand::JobsCommand(Output *output, JobsRegistry &registry)
    : BuiltinCommand("jobs", "jobs is a shell builtin", output),
      m_registry(registry)
{
}

int JobsCommand::Process(const std::vector<Token> &tokens) const
{
    std::map<unsigned int, BackgroundJob> jobs = m_registry.GetAll();
    for (auto it = jobs.begin(); it != jobs.end(); it++)
    {
        char job_marker = ' ';
        if (std::next(it) == jobs.end())
        {
            job_marker = '+';
        }
        if (std::next(it) == std::prev(jobs.end()))
        {
            job_marker = '-';
        }

        // Status column (Running) is padded to 24 characters (7 characters + 17
        // characters space)
        m_output->Put(tokens,
                      std::format("[{}]{}  Running                 {}\n",
                                  it->first, job_marker,
                                  it->second.commandline),
                      OutputTarget::STDOUT);
    }

    return 0;
}
