#include "jobs.h"
#include "command.h"

JobsCommand::JobsCommand(Output *output)
    : BuiltinCommand("jobs", "jobs is a shell builtin", output)
{
}

void JobsCommand::Process(const std::vector<Token> &tokens) const {}
