#include "redirect_stderr.h"

#include <algorithm>
#include <fstream>
#include <iostream>

using Ast::Redirect;
using Ast::TokenType;

void RedirectStdErr::Print(const Ast::Command &comm,
                           const std::string &out_buffer) const
{
    const Redirect *redirect = GetStdErr(comm);

    if (redirect == nullptr || redirect->Target.empty())
    {
        return;
    }

    std::ios::openmode mode = std::ios::app;
    if (redirect->Type == TokenType::TWO_GREATER)
    {
        mode = std::ios::out | std::ios::trunc;
    }
    std::ofstream file(redirect->Target, mode);

    file << out_buffer;
    if (!file.is_open())
    {
        std::cerr << "Error: cannot open " << redirect->Target << '\n';
    }

    file.flush();
    file.close();
}

bool RedirectStdErr::IsApplicable(const Ast::Command &comm,
                                  OutputTarget target) const
{
    return std::ranges::any_of(
               comm.Redirects,
               [](const Redirect &redir) {
                   return redir.Type == TokenType::TWO_GREATER ||
                          redir.Type == TokenType::TWO_GREATER_GREATER;
               }) &&
           (target == OutputTarget::STDERR);
}

const Redirect *RedirectStdErr::GetStdErr(const Ast::Command &comm)
{
    for (const auto &redirect : comm.Redirects)
    {
        if (redirect.Type == TokenType::TWO_GREATER ||
            redirect.Type == TokenType::TWO_GREATER_GREATER)
        {
            return &redirect;
        }
    }

    return nullptr;
}
