#include "complete.h"

#include <format>

void CompleteCommand::Process(const std::vector<Token> &tokens) const
{
    std::string printFlag;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        if (i == 0 || tokens[i - 1].type != TokenType::FLAG)
        {
            continue;
        }

        if (tokens[i - 1].token.starts_with("-p"))
        {
            printFlag = tokens[i].token;
        }
    }

    m_output->Put(
        tokens,
        std::format("complete: {}: no completion specification\n", printFlag),
        OutputTarget::STDERR);
}

CompleteCommand::CompleteCommand(Output *output)
    : BuiltinCommand("complete", "complete is a shell builtin", output)
{
}
