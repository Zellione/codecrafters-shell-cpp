#include "exit.h"
#include "builtin_command.h"

ExitCommand::ExitCommand()
    : BuiltinCommand("exit", "exit is a shell builtin") {}

void ExitCommand::Process(std::vector<Node> arguments) const { exit(0); }
