#include <iostream>

#include "err_not_found.h"

using Ast::Command;

void ErrorNotFound::Raise(const Command &comm)
{
    std::string command = !comm.Args.empty() ? comm.Args[0] : "";
    std::cerr << command << ": command not found" << '\n';
}
