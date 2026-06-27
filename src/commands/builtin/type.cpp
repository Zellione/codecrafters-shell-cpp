#include "type.h"
#include "../../helper.h"
#include "command.h"

#include <iostream>
#include <unistd.h>

TypeCommand::TypeCommand(const BuiltinRegistry *registry, Output *output)
    : BuiltinCommand("type", "type is a shell builtin", output),
      m_registry(registry)
{
}

void TypeCommand::Process(const std::vector<Token> &tokens) const
{
    const BuiltinCommand *command =
        m_registry->FindCommandInArguments(tokens, 1);

    if (command != nullptr)
    {
        std::cout << command->GetDescription() << '\n';
        return;
    }

    std::string type_check = !tokens.empty() ? tokens[1].token : "";
    std::string executable = find_executable(type_check);
    if (!executable.empty())
    {
        std::cout << type_check << " is " << executable << '\n';
        return;
    }

    std::cout << type_check << ": not found" << '\n';
}
