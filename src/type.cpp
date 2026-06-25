#include "type.h"
#include "builtin_command.h"
#include "helper.h"

#include <iostream>
#include <unistd.h>

TypeCommand::TypeCommand(const BuiltinRegistry *registry, Output *output)
    : BuiltinCommand("type", "type is a shell builtin", output),
      m_registry(registry) {}

void TypeCommand::Process(std::vector<Token> tokens) const {
    std::string type_check = tokens.size() > 0 ? tokens[1].token : "";

    const BuiltinCommand *command = m_registry->FindCommand(type_check);

    if (command != nullptr) {
        std::cout << command->GetDescription() << std::endl;
        return;
    }

    std::string executable = find_executable(type_check);
    if (executable != "") {
        std::cout << type_check << " is " << executable << std::endl;
        return;
    }

    std::cout << type_check << ": not found" << std::endl;
}
