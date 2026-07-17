#pragma once

#include <map>
#include <string>
#include <vector>

#include "../commands/executor.h"

class CompleteRegistry
{
  private:
    std::map<std::string, std::string> m_completions;
    Executor& m_executor;

    [[nodiscard]] Ast::Node *
    BuildAutocompleteTokens(const Ast::Command &comm,
                            const std::string &partial) const;
    [[nodiscard]] static std::vector<char *>
    BuildEnvVars(const std::string &partial);

  public:
    CompleteRegistry(Executor &executor);

    void Add(const std::string &name, const std::string &completion);
    void Remove(const std::string &name);
    [[nodiscard]] const std::string &Get(const std::string &name) const;
    [[nodiscard]] bool Has(const std::string &name) const;

    [[nodiscard]] std::vector<std::string>
    Autocomplete(const Ast::Command &comm, const std::string &partial) const;
};
