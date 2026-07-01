#include "jobs.h"
#include "command.h"

JobsCommand::JobsCommand(Output *output, JobsRegistry &registry)
    : BuiltinCommand("jobs", "jobs is a shell builtin", output),
      m_registry(registry)
{
}

void JobsCommand::Process(const std::vector<Token> &tokens) const {}
