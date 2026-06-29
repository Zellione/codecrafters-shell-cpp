#include "complete.h"

void CompleteCommand::Process(const std::vector<Token> &tokens) const {}

CompleteCommand::CompleteCommand(Output *output)
    : BuiltinCommand("complete", "complete is a shell builtin", output)
{
}
