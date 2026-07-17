#include "registry.h"
#include "command.h"

using Ast::Command;
using std::string;

BuiltinRegistry::~BuiltinRegistry()
{
    for (auto *command : m_commands)
    {
        delete command;
        command = nullptr;
    }
}

const BuiltinCommand *BuiltinRegistry::FindCommand(const Command &comm) const
{
    return FindCommandInArguments(comm, 0);
}

const BuiltinCommand *
BuiltinRegistry::FindCommandInArguments(const Command &comm, size_t pos) const
{
    string command = !comm.Args.empty() ? comm.Args[pos] : "";

    for (auto *const builtin_com : m_commands)
    {
        if (builtin_com->IsCommand(command))
        {
            return builtin_com;
        }
    }

    return nullptr;
}

std::vector<string> BuiltinRegistry::AutoComplete(const string &partial) const
{
    std::vector<string> commands;
    for (auto *const builtin_com : m_commands)
    {
        if (builtin_com->NameStartWith(partial))
        {
            commands.push_back(builtin_com->GetName());
        }
    }

    return commands;
}

void BuiltinRegistry::RegisterCommand(BuiltinCommand *command)
{
    m_commands.push_back(command);
}
