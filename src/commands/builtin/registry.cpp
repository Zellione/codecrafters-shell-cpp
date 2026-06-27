#include "registry.h"
#include "command.h"

using std::string;

BuiltinRegistry::~BuiltinRegistry()
{
    for (auto *command : m_commands)
    {
        delete command;
        command = nullptr;
    }
}

const BuiltinCommand *
BuiltinRegistry::FindCommand(const std::vector<Token> &tokens) const
{
    return FindCommandInArguments(tokens, 0);
}

const BuiltinCommand *
BuiltinRegistry::FindCommandInArguments(const std::vector<Token> &tokens,
                                        size_t pos) const
{
    string command = !tokens.empty() ? tokens[pos].token : "";

    for (auto *const builtin_com : m_commands)
    {
        if (builtin_com->IsCommand(command))
        {
            return builtin_com;
        }
    }

    return nullptr;
}

string BuiltinRegistry::AutoComplete(const string &partial) const
{
    for (auto *const builtin_com : m_commands)
    {
        if (builtin_com->NameStartWith(partial))
        {
            return builtin_com->GetName();
        }
    }

    return partial;
}

void BuiltinRegistry::RegisterCommand(BuiltinCommand *command)
{
    m_commands.push_back(command);
}
