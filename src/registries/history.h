#pragma once

#include <string>
#include <vector>

class HistoryRegistry
{
  private:
    std::vector<std::string> m_history;

  public:
    HistoryRegistry() = default;

    void Add(const std::string &commandline);
    [[nodiscard]] const std::vector<std::string> &Get() const;
};
