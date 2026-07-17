#include "console_error.h"
#include <iostream>

void ConsoleError::Print(const Ast::Command &comm,
                         const std::string &out_buffer) const
{
    std::cerr << out_buffer;
}

bool ConsoleError::IsApplicable(const Ast::Command &comm,
                                OutputTarget target) const
{
    return target == OutputTarget::STDERR;
}
