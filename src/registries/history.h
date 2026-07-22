#pragma once

#include <string>
#include <vector>

class HistoryRegistry
{
  private:
    std::vector<std::string> m_history;
    size_t m_current_pos;
    size_t m_next_exported_pos;

  public:
    HistoryRegistry();

    void Add(const std::string &commandline);
    [[nodiscard]] const std::vector<std::string> &Get() const;

    [[nodiscard]] std::string Up();
    [[nodiscard]] std::string Down();

    void MarkCurrentExportedPos();
    [[nodiscard]] std::vector<std::string> GetNotExportedSubset();
};
