#include "pwd.h"
#include <filesystem>

using Ast::Command;

PwdCommand::PwdCommand() : BuiltinCommand("pwd") {}

int PwdCommand::Process(const Command &comm) {
    std::cout << std::format("{}\n", std::filesystem::current_path().c_str());

    return 0;
}
