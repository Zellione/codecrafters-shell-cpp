#include "redirect_std_out.h"
#include <fstream>
#include <iostream>

void RedirectStdOut::Print(const std::vector<Token> &tokens,
                           const char *out_buffer) const {
    std::string output_name = GetStdOut(tokens);

    if (output_name.empty())
        return;

    std::ofstream file(output_name, std::ios::app);

    file << out_buffer;
    if (!file.is_open()) {
        std::cout << "Error: cannot open " << output_name << std::endl;
    }

    file.flush();
}

bool RedirectStdOut::IsApplicable(const std::vector<Token> &tokens) const {
    for (const auto &token : tokens) {
        if (token.type == TokenType::REDIRECT_STDOUT)
            return true;
    }

    return false;
}

std::string RedirectStdOut::GetStdOut(const std::vector<Token> &tokens) const {
    for (const auto &token : tokens) {
        if (token.type == TokenType::REDIRECT_STDOUT)
            return token.token;
    }

    return "";
}
