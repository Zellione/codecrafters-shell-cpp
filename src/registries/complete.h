#pragma once

#include <map>
#include <string>
#include <vector>

#include "../commands/external/command.h"

class CompleteRegistry
{
  private:
    std::map<std::string, std::string> m_completions;

    [[nodiscard]] std::vector<Token>
    BuildAutocompleteTokens(const std::vector<Token> &tokens,
                            const std::string &partial) const;
    [[nodiscard]] std::vector<char *>
    BuildEnvVars(const std::vector<Token> &tokens,
                 const std::string &partial) const;
    ExternalCommand *m_externalCommand;

  public:
    CompleteRegistry(ExternalCommand *externalCommand);

    void Add(const std::string &name, const std::string &completion);
    void Remove(const std::string &name);
    [[nodiscard]] const std::string &Get(const std::string &name) const;
    [[nodiscard]] bool Has(const std::string &name) const;

    [[nodiscard]] std::vector<std::string>
    Autocomplete(const std::vector<Token> &tokens,
                 const std::string &partial) const;
};
