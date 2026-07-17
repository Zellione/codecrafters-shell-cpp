#include "complete.h"

#include <cstring>
#include <format>
#include <ranges>

#include "../parser/lexer.h"
#include "../parser/parser.h"

using Ast::Command;
using Ast::Node;
using std::format;
using std::string;
using std::vector;

CompleteRegistry::CompleteRegistry(Executor &executor)
    : m_executor(executor)
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

vector<string> CompleteRegistry::Autocomplete(const Command &comm,
                                              const string &partial) const
{
    if (comm.Args.empty() || !Has(comm.Args[0]))
    {
        return {};
    }

    Node *nodes = BuildAutocompleteTokens(comm, partial);
    vector<char *> env_vars = BuildEnvVars(partial);

    vector<string> autocompletions;
    CmdResult result;
    if (m_executor.Exec(nodes, env_vars, &result) == 0)
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

Node *CompleteRegistry::BuildAutocompleteTokens(const Command &comm,
                                                const string &partial) const
{
    const string &completion_script = Get(comm.Args[0]);

    string complete_comm = std::format(
        R"({} "{}" "{}" "{}")", completion_script, comm.Args[0],
        comm.Args.empty() ? "" : comm.Args.back(),
        comm.Args.size() > 1 ? comm.Args[comm.Args.size() - 2] : "");

    Lexer comp_lexer(complete_comm);
    Parser comp_parser(comp_lexer);
    return comp_parser.Parse();
}

vector<char *> CompleteRegistry::BuildEnvVars(const string &partial)
{
    vector<char *> env;
    env.push_back(strdup(format("COMP_LINE={}", partial).c_str()));
    env.push_back(strdup(format("COMP_POINT={}", partial.length()).c_str()));
    env.push_back(nullptr);

    return env;
}
