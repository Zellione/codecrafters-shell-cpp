#include "console_output.h"
#include <iostream>

void ConsoleOutput::Print(const std::vector<Token> &tokens,
                          const std::string &out_buffer) const {

    std::cout << out_buffer;
}

bool ConsoleOutput::IsApplicable(const std::vector<Token> &tokens,
                                 OutputTarget target) const {
    return true;
}
