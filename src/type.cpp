#include "type.h"
#include "builtin_command.h"
#include "helper.h"

#include <iostream>
#include <unistd.h>

TypeCommand::TypeCommand(const BuiltinRegistry *registry, Output *output)
    : BuiltinCommand("type", "type is a shell builtin", output),
      m_registry(registry) {}

void TypeCommand::Process(const std::vector<Token> &tokens) const {
    std::string type_check = !tokens.empty() ? tokens[1].token : "";

    const BuiltinCommand *command = m_registry->FindCommand(type_check);

    if (command != nullptr) {
        std::cout << command->GetDescription() << '\n';
        return;
    }

    std::string executable = find_executable(type_check);
    if (!executable.empty()) {
        std::cout << type_check << " is " << executable << '\n';
        return;
    }

    std::cout << type_check << ": not found" << '\n';
}
