#include "command.h"

#include <utility>

using std::string;

BuiltinCommand::BuiltinCommand(string name, string description)
    : m_name(std::move(name)), m_description(std::move(description))
{
}

bool BuiltinCommand::IsCommand(const string &command) const
{
    return command.starts_with(m_name);
}

bool BuiltinCommand::NameStartWith(const std::string &partial) const
{
    return m_name.starts_with(partial);
}

int BuiltinCommand::Process(const Ast::Command &command) const { return 0; }

int BuiltinCommand::Execute(const Ast::Command &command) const
{
    return Process(command);
}

const std::string &BuiltinCommand::GetName() const { return m_name; }
const std::string &BuiltinCommand::GetDescription() const
{
    return m_description;
}
