#include "complete.h"
#include <ranges>

CompleteRegistry::CompleteRegistry(ExternalCommand *externalCommand)
    : m_externalCommand(externalCommand)
{
}

void CompleteRegistry::Add(const std::string &name,
                           const std::string &completion)
{
    m_completions.emplace(name, completion);
}

const std::string &CompleteRegistry::Get(const std::string &name) const
{
    return m_completions.find(name)->second;
}

bool CompleteRegistry::Has(const std::string &name) const
{
    return m_completions.contains(name);
}

std::vector<std::string>
CompleteRegistry::Autocomplete(const std::vector<Token> &tokens,
                               const std::string &partial) const
{
    if (tokens.empty() || !Has(tokens[0].token))
    {
        return {};
    }
    const std::string &completion_script = Get(tokens[0].token);

    std::vector<Token> comp_tokens = TokenParser::Parse(completion_script);

    CmdResult result;
    if (m_externalCommand->Exec(comp_tokens, &result))
    {
        if (result.stdout_output.ends_with('\n'))
        {
            result.stdout_output.pop_back();
        }
        auto parts = result.stdout_output | std::views::split('\n');
        std::vector<std::string> autocompletions =
            parts | std::ranges::to<std::vector<std::string>>();

        return autocompletions;
    }

    return {};
}
