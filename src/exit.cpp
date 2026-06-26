#include "exit.h"
#include "builtin_command.h"

ExitCommand::ExitCommand(Output *output)
    : BuiltinCommand("exit", "exit is a shell builtin", output) {}

void ExitCommand::Process(const std::vector<Token> &tokens) const { exit(0); }
