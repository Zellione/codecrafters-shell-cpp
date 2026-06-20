#include "builtin_command.h"
#include "helper.h"

BuiltinCommand::BuiltinCommand(std::string name, std::string description)
    : m_name(name), m_description(description) {}

bool BuiltinCommand::IsCommand(std::string command) const {
    return command.substr(0, m_name.length()) == m_name;
}

void BuiltinCommand::Execute(std::string commandline) const {
    std::vector<std::string> arguments = get_command_and_args(commandline);

    Process(arguments);
}

std::string BuiltinCommand::GetDescription() const { return m_description; }
