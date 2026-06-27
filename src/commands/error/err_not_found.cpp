#include <iostream>

#include "err_not_found.h"

void ErrorNotFound::Raise(const std::vector<Token> &tokens)
{
    std::string command = !tokens.empty() ? tokens[0].token : "";
    std::cout << command << ": command not found" << '\n';
}
