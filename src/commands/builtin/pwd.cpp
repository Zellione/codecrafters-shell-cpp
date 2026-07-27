#include "pwd.h"

using Ast::Command;

PwdCommand::PwdCommand(const std::string &current_directory)
    : BuiltinCommand("pwd"), m_current_directory(current_directory)
{
}

int PwdCommand::Process(const Command &comm)
{
    std::cout << std::format("{}\n", m_current_directory);

    return 0;
}
