#include "builtin_command.h"

BuiltinCommand::BuiltinCommand(std::string name, std::string description,
                               Output *output)
    : m_name(name), m_description(description), m_output(output), m_parser() {}

bool BuiltinCommand::IsCommand(std::string command) const {
    return command.substr(0, m_name.length()) == m_name;
}

void BuiltinCommand::Execute(std::string commandline) const {
    std::vector<Token> arguments = m_parser.Parse(commandline);

    Process(arguments);
}

std::string BuiltinCommand::GetDescription() const { return m_description; }
