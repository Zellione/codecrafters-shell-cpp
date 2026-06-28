#include "echo.h"
#include "../../parser/token_parser.h"
#include <sstream>

EchoCommand::EchoCommand(Output *output)
    : BuiltinCommand("echo", "echo is a shell builtin", output)
{
}

void EchoCommand::Process(const std::vector<Token> &tokens) const
{
    bool trailing_newline = true;

    std::stringstream ss;
    for (size_t i = 1; i < tokens.size(); i++)
    {
        if (tokens[i].token.starts_with("-n"))
        {
            trailing_newline = false;
            continue;
        }

        if (tokens[i].type == TokenType::TEXT)
        {
            ss << tokens[i].token;
            if (i < tokens.size() - 1 && tokens[i + 1].type == TokenType::TEXT)
            {
                ss << " ";
            }
        }
    }
    if (trailing_newline)
    {
        ss << '\n';
    }

    m_output->Put(tokens, ss.str(), OutputTarget::STDOUT);
    m_output->Put(tokens, "", OutputTarget::STDERR);
}
