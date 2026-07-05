#include "redirect_stdout.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

void RedirectStdOut::Print(const std::vector<Token> &tokens,
                           const std::string &out_buffer) const
{
    const Token *output_token = GetStdOut(tokens);

    if (output_token == nullptr || output_token->token.empty())
    {
        return;
    }

    if (output_token->type == TokenType::REDIRECT_STDOUT)
    {
        if (std::filesystem::exists(output_token->token))
        {
            std::filesystem::remove(output_token->token);
        }
    }
    std::ofstream file(output_token->token, std::ios::app);

    file << out_buffer;
    if (!file.is_open())
    {
        std::cout << "Error: cannot open " << output_token->token << '\n';
    }

    file.flush();
    file.close();
}

bool RedirectStdOut::IsApplicable(const std::vector<Token> &tokens,
                                  OutputTarget target) const
{
    return std::ranges::any_of(
               tokens,
               [](const Token &token) {
                   return token.type == TokenType::REDIRECT_STDOUT ||
                          token.type == TokenType::REDIRECT_STDOUT_APPEND;
               }) &&
           (target == OutputTarget::STDOUT);
}

const Token *RedirectStdOut::GetStdOut(const std::vector<Token> &tokens)
{
    for (const auto &token : tokens)
    {
        if (token.type == TokenType::REDIRECT_STDOUT ||
            token.type == TokenType::REDIRECT_STDOUT_APPEND)
        {
            return &token;
        }
    }

    return nullptr;
}
