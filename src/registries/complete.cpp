#include "complete.h"
#include <cstring>
#include <format>
#include <ranges>

using std::format;
using std::string;
using std::vector;

CompleteRegistry::CompleteRegistry(ExternalCommand *externalCommand)
    : m_externalCommand(externalCommand)
{
}

void CompleteRegistry::Add(const string &name, const string &completion)
{
    m_completions.emplace(name, completion);
}

void CompleteRegistry::Remove(const std::string &name)
{
    m_completions.erase(name);
}

const string &CompleteRegistry::Get(const std::string &name) const
{
    return m_completions.find(name)->second;
}

bool CompleteRegistry::Has(const string &name) const
{
    return m_completions.contains(name);
}

vector<string> CompleteRegistry::Autocomplete(const vector<Token> &tokens,
                                              const string &partial) const
{
    if (tokens.empty() || !Has(tokens[0].token))
    {
        return {};
    }

    vector<Token> comp_tokens = BuildAutocompleteTokens(tokens, partial);
    vector<char *> env_vars = BuildEnvVars(tokens, partial);

    vector<string> autocompletions;
    CmdResult result;
    if (m_externalCommand->Exec(comp_tokens, env_vars, &result))
    {
        if (result.stdout_output.ends_with('\n'))
        {
            result.stdout_output.pop_back();
        }
        auto parts = result.stdout_output | std::views::split('\n');
        autocompletions = parts | std::ranges::to<vector<string>>();
    }

    return autocompletions;
}

vector<Token>
CompleteRegistry::BuildAutocompleteTokens(const vector<Token> &tokens,
                                          const string &partial) const
{
    const string &completion_script = Get(tokens[0].token);

    vector<Token> comp_tokens = TokenParser::Parse(completion_script);

    string complete_comm =
        std::format(R"({} "{}" "{}" "{}")", completion_script, tokens[0].token,
                    tokens.empty() ? "" : tokens.back().token,
                    tokens.size() > 1 ? tokens[tokens.size() - 2].token : "");

    return TokenParser::Parse(complete_comm);
}

vector<char *> CompleteRegistry::BuildEnvVars(const vector<Token> &tokens,
                                              const string &partial) const
{
    vector<char *> env;
    env.push_back(strdup(format("COMP_LINE={}", partial).c_str()));
    env.push_back(strdup(format("COMP_POINT={}", partial.length()).c_str()));
    env.push_back(nullptr);

    return env;
}
