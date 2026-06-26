#include "redirect_err_out.h"
#include <algorithm>
#include <fstream>
#include <iostream>

void RedirectErrOut::Print(const std::vector<Token> &tokens,
                           const std::string &out_buffer) const {
    std::string output_name = GetErrOut(tokens);

    if (output_name.empty()) {
        return;
    }

    std::ofstream file(output_name, std::ios::app);

    file << out_buffer;
    if (!file.is_open()) {
        std::cout << "Error: cannot open " << output_name << '\n';
    }

    file.flush();
}

bool RedirectErrOut::IsApplicable(const std::vector<Token> &tokens,
                                  OutputTarget target) const {
    return std::ranges::any_of(tokens,
                               [](const Token &token) {
                                   return token.type ==
                                          TokenType::REDIRECT_STDERR;
                               }) &&
           (target == OutputTarget::ERROUT || target == OutputTarget::NONE);
}

std::string RedirectErrOut::GetErrOut(const std::vector<Token> &tokens) {
    for (const auto &token : tokens) {
        if (token.type == TokenType::REDIRECT_STDERR) {
            return token.token;
        }
    }

    return "";
}
