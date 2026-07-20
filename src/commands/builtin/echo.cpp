#include "echo.h"
#include <sstream>

using Ast::Command;

EchoCommand::EchoCommand() : BuiltinCommand("echo")
{
}

int EchoCommand::Process(const Command &comm) const
{
    bool trailing_newline = true;

    std::stringstream ss;
    for (size_t i = 1; i < comm.Args.size(); i++)
    {
        if (comm.Args[i].starts_with("-n"))
        {
            trailing_newline = false;
            continue;
        }

        ss << comm.Args[i];
        if (i < comm.Args.size() - 1)
        {
            ss << " ";
        }
    }
    if (trailing_newline)
    {
        ss << '\n';
    }

    std::cout << ss.str();

    return 0;
}
