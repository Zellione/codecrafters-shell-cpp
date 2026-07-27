#pragma once

#include <map>
#include <string>

class VariableRegistry
{
  private:
    std::map<std::string, std::string> m_variables;

  public:
    void AddVariable(const std::string &key, const std::string &value);
    [[nodiscard]] std::string GetVariable(const std::string &key) const;
};
