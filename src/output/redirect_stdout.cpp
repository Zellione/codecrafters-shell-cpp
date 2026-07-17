#include "redirect_stdout.h"

#include <algorithm>
#include <fstream>
#include <iostream>

using Ast::Command;
using Ast::Redirect;
using Ast::TokenType;

void RedirectStdOut::Print(const Ast::Command &comm,
                           const std::string &out_buffer) const
{
    const Redirect *redir = GetStdOut(comm);

    if (redir == nullptr || redir->Target.empty())
    {
        return;
    }

    std::ios::openmode mode = std::ios::app;
    if (redir->Type == TokenType::GREATER ||
        redir->Type == Ast::TokenType::ONE_GREATER)
    {
        mode = std::ios::out | std::ios::trunc;
    }
    std::ofstream file(redir->Target, mode);

    file << out_buffer;
    if (!file.is_open())
    {
        std::cout << "Error: cannot open " << redir->Target << '\n';
    }

    file.flush();
    file.close();
}

bool RedirectStdOut::IsApplicable(const Ast::Command &comm,
                                  OutputTarget target) const
{
    return std::ranges::any_of(
               comm.Redirects,
               [](const Redirect &redir) {
                   return redir.Type == TokenType::GREATER ||
                          redir.Type == TokenType::GREATER_GREATER ||
                          redir.Type == TokenType::ONE_GREATER ||
                          redir.Type == TokenType::ONE_GREATER_GREATER;
               }) &&
           (target == OutputTarget::STDOUT);
}

const Redirect *RedirectStdOut::GetStdOut(const Command &comm)
{
    for (const auto &redirect : comm.Redirects)
    {
        if (redirect.Type == Ast::TokenType::GREATER ||
            redirect.Type == Ast::TokenType::GREATER_GREATER ||
            redirect.Type == Ast::TokenType::ONE_GREATER ||
            redirect.Type == Ast::TokenType::ONE_GREATER_GREATER)
        {
            return &redirect;
        }
    }

    return nullptr;
}
