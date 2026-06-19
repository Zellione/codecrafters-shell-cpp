#include "builtin_command.h"

BuiltinCommand::BuiltinCommand(std::string name, std::string description)
    : m_name(name), m_description(description) {}

bool BuiltinCommand::IsCommand(std::string command) const {
  return command.substr(0, m_name.length()) == m_name;
}

void BuiltinCommand::Execute(std::string command) const {
  std::vector<std::string> arguments;

  size_t pos = m_name.length();
  while (pos < command.length()) {
    if (command[pos] == ' ')
      pos++;

    size_t next = command.find(' ', pos);
    if (next == std::string::npos) {
      arguments.push_back(command.substr(pos));
      break;
    }
    size_t arg_length = next - pos;
    arguments.push_back(command.substr(pos, arg_length));
    pos = next;
  }

  Process(arguments);
}

std::string BuiltinCommand::GetDescription() const { return m_description; }
