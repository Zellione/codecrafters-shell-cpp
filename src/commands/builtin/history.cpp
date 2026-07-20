#include "history.h"
#include "../../shell.h"
#include "command.h"

using Ast::Command;

HistoryCommand::HistoryCommand(HistoryRegistry *registry)
    : BuiltinCommand("history"), m_registry(registry)
{
}

int HistoryCommand::Process(const Command &comm) const
{
    const auto &entries = m_registry->Get();
    for (size_t i = 0; i < entries.size(); i++)
    {
        std::cout << std::format("    {}  {}\n", i + 1, entries[i]);
    }

    return 0;
}
