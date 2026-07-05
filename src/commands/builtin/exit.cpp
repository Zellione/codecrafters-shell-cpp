#include "exit.h"
#include "../../shell.h"
#include "command.h"

ExitCommand::ExitCommand(Output *output)
    : BuiltinCommand("exit", "exit is a shell builtin", output)
{
}

int ExitCommand::Process(const std::vector<Token> &tokens) const
{
    Shell &shell = Shell::Instance();
    shell.ExitShell(true);
    return 0;
}
