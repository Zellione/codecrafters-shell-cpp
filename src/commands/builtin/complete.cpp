#include "complete.h"

#include <format>

void CompleteCommand::Process(const std::vector<Token> &tokens) const
{
    std::string name;
    std::string completion;
    bool flag_print = false;
    bool flag_create = false;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        switch (tokens[i].type)
        {
        case TokenType::FLAG:
            // printFlag
            if (tokens[i].token == "-p")
            {
                flag_print = true;
            }

            // addFlag
            if (tokens[i].token == "-C")
            {
                if (i >= tokens.size())
                {
                    continue;
                }
                flag_create = true;
            }
            break;
        case TokenType::FILE_PATH:
            completion = tokens[i].token;
            break;
        case TokenType::TEXT:
            name = tokens[i].token;
        default:
            break;
        }
    }

    if (flag_print)
    {
        Print(tokens, name);
        return;
    }

    if (flag_create)
    {
        Create(name, completion);
    }
}

void CompleteCommand::Print(const std::vector<Token> &tokens,
                            const std::string &name) const
{
    if (!m_completeRegistry->Has(name))
    {
        m_output->Put(
            tokens,
            std::format("complete: {}: no completion specification\n", name),
            OutputTarget::STDERR);

        return;
    }

    m_output->Put(tokens,
                  std::format("complete -C '{}' {}\n",
                              m_completeRegistry->Get(name), name),
                  OutputTarget::STDOUT);
}

bool CompleteCommand::Create(const std::string &name,
                             const std::string &completion) const
{
    if (m_completeRegistry->Has(name))
    {
        return false;
    }

    m_completeRegistry->Add(name, completion);

    return true;
}

CompleteCommand::CompleteCommand(Output *output,
                                 CompleteRegistry *completeRegistry)
    : BuiltinCommand("complete", "complete is a shell builtin", output),
      m_completeRegistry(completeRegistry)
{
}
