#include "console_output.h"
#include <iostream>

void ConsoleOutput::Print(const std::vector<Token> &tokens,
                          const char *out_buffer) const {

    std::cout << out_buffer;
}

bool ConsoleOutput::IsApplicable(const std::vector<Token> &tokens) const {
    return true;
}
