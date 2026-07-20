#include "history.h"
#include "../../shell.h"
#include "command.h"

using Ast::Command;

HistoryCommand::HistoryCommand() : BuiltinCommand("history")
{
}

int HistoryCommand::Process(const Command &comm) const
{
    return 0;
}
