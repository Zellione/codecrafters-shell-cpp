#include "history.h"

void HistoryRegistry::Add(const std::string &commandline)
{
    m_history.push_back(commandline);
}

[[nodiscard]] const std::vector<std::string> &HistoryRegistry::Get() const
{
    return m_history;
}
