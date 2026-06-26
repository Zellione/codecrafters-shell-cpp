#include "redirect_std_err.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

void RedirectStdErr::Print(const std::vector<Token> &tokens,
                           const std::string &out_buffer) const {
    const Token *output_token = GetStdErr(tokens);

    if (output_token == nullptr || output_token->token.empty()) {
        return;
    }

    if (output_token->type == TokenType::REDIRECT_STDDERR) {
        if (std::filesystem::exists(output_token->token)) {
            std::filesystem::remove(output_token->token);
        }
    }
    std::ofstream file(output_token->token, std::ios::app);

    file << out_buffer;
    if (!file.is_open()) {
        std::cout << "Error: cannot open " << output_token->token << '\n';
    }

    file.flush();
}

bool RedirectStdErr::IsApplicable(const std::vector<Token> &tokens,
                                  OutputTarget target) const {
    return std::ranges::any_of(
               tokens,
               [](const Token &token) {
                   return token.type == TokenType::REDIRECT_STDDERR ||
                          token.type == TokenType::REDIRECT_STDERR_APPEND;
               }) &&
           (target == OutputTarget::STDERR || target == OutputTarget::NONE);
}

const Token *RedirectStdErr::GetStdErr(const std::vector<Token> &tokens) {
    for (const auto &token : tokens) {
        if (token.type == TokenType::REDIRECT_STDDERR ||
            token.type == TokenType::REDIRECT_STDERR_APPEND) {
            return &token;
        }
    }

    return nullptr;
}
