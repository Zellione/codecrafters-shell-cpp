#include "console_output.h"
#include <iostream>

void ConsoleOutput::Print(const Ast::Command &comm,
                          const std::string &out_buffer) const
{
    std::cout << out_buffer;
}

bool ConsoleOutput::IsApplicable(const Ast::Command &comm,
                                 OutputTarget target) const
{
    return target == OutputTarget::STDOUT;
}
