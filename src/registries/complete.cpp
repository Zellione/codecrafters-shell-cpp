#include "complete.h"
#include <format>
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

    std::vector<Token> comp_tokens = BuildAutocompleteTokens(tokens, partial);

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

std::vector<Token>
CompleteRegistry::BuildAutocompleteTokens(const std::vector<Token> &tokens,
                                          const std::string &partial) const
{
    const std::string &completion_script = Get(tokens[0].token);

    std::vector<Token> comp_tokens = TokenParser::Parse(completion_script);

    std::string complete_comm =
        std::format(R"({} "{}" "{}" "{}")", completion_script, tokens[0].token,
                    tokens.empty() ? "" : tokens.back().token,
                    tokens.size() > 2 ? tokens[tokens.size() - 2].token : "");

    return TokenParser::Parse(complete_comm);
}
