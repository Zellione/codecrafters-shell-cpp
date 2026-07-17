#include "type.h"
#include "../../helper.h"
#include "command.h"

using Ast::Command;

TypeCommand::TypeCommand(const BuiltinRegistry *registry)
    : BuiltinCommand("type", "type is a shell builtin"), m_registry(registry)
{
}

int TypeCommand::Process(const Command &comm) const
{
    const BuiltinCommand *command = m_registry->FindCommandInArguments(comm, 1);

    if (command != nullptr)
    {
        std::cout << std::format("{}\n", command->GetDescription());

        return 0;
    }

    std::string type_check = !comm.Args.empty() ? comm.Args[1] : "";
    std::string executable = find_executable(type_check);
    if (!executable.empty())
    {
        std::cout << std::format("{} is {}\n", type_check, executable);

        return 0;
    }

    std::cerr << std::format("{}: not found\n", type_check);

    return 0;
}
