#include "console_error.h"
#include <iostream>

void ConsoleError::Print(const std::vector<Token> &tokens,
                         const std::string &out_buffer) const
{
    std::cerr << out_buffer;
}

bool ConsoleError::IsApplicable(const std::vector<Token> &tokens,
                                OutputTarget target) const
{
    return target == OutputTarget::STDERR;
}
