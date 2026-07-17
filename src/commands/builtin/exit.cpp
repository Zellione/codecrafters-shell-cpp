#include "exit.h"
#include "../../shell.h"
#include "command.h"

using Ast::Command;

ExitCommand::ExitCommand() : BuiltinCommand("exit", "exit is a shell builtin")
{
}

int ExitCommand::Process(const Command &comm) const
{
    Shell &shell = Shell::Instance();
    shell.ExitShell(true);
    return 0;
}
