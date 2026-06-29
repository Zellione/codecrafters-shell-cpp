#pragma once

#include <map>
#include <string>

class CompleteRegistry
{
  private:
    std::map<std::string, std::string> m_completions;

  public:
    CompleteRegistry() = default;

    void Add(const std::string &name, const std::string &completion);
    [[nodiscard]] const std::string &Get(const std::string &name) const;
    [[nodiscard]] bool Has(const std::string &name) const;
};
