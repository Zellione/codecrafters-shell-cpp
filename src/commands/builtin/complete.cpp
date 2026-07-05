#include "complete.h"

#include <format>

int CompleteCommand::Process(const std::vector<Token> &tokens) const
{
    std::string name;
    std::string completion;
    bool flag_print = false;
    bool flag_create = false;
    bool flag_remove = false;

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

            // removeFlag
            if (tokens[i].token == "-r")
            {
                flag_remove = true;
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

    if (flag_remove)
    {
        if (m_completeRegistry->Has(name))
        {
            m_completeRegistry->Remove(name);
        }
    }

    if (flag_print)
    {
        Print(tokens, name);
        return 0;
    }

    if (flag_create)
    {
        Create(name, completion);
    }

    return 0;
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

void CompleteCommand::Create(const std::string &name,
                             const std::string &completion) const
{
    if (m_completeRegistry->Has(name))
    {
        return;
    }

    m_completeRegistry->Add(name, completion);
}

CompleteCommand::CompleteCommand(Output *output,
                                 CompleteRegistry *completeRegistry)
    : BuiltinCommand("complete", "complete is a shell builtin", output),
      m_completeRegistry(completeRegistry)
{
}
