#include "builtin_command.h"

#include <utility>

using std::string;

BuiltinCommand::BuiltinCommand(string name, string description, Output *output)
    : m_name(std::move(name)), m_description(std::move(description)),
      m_output(output) {}

bool BuiltinCommand::IsCommand(const string &command) const {
    return command.starts_with(m_name);
}

void BuiltinCommand::Execute(const string &commandline) const {
    std::vector<Token> arguments = TokenParser::Parse(commandline);

    Process(arguments);
}

std::string BuiltinCommand::GetDescription() const { return m_description; }
