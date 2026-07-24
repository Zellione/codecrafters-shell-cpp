#include "declare.h"

using Ast::Command;

DeclareCommand::DeclareCommand() : BuiltinCommand("declare") {}

int DeclareCommand::Process(const Command &comm) const
{
    std::string variable_name;
    for (size_t i = 1; i < comm.Args.size(); i++)
    {
        if (comm.Args[i] == "-p" && (i + 1) < comm.Args.size())
        {
            variable_name = comm.Args[++i];
        }
    }

    std::cout << std::format("declare: {}: not found\n", variable_name);

    return 0;
}
