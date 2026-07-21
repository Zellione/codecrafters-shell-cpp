#include "history.h"

#include <algorithm>

HistoryRegistry::HistoryRegistry() : m_current_pos(0) {}

void HistoryRegistry::Add(const std::string &commandline)
{
    m_history.push_back(commandline);
    m_current_pos++;
}

const std::vector<std::string> &HistoryRegistry::Get() const
{
    return m_history;
}

std::string HistoryRegistry::Up()
{
    m_current_pos = m_current_pos > 1 ? m_current_pos - 1 : 0;

    return m_history[m_current_pos];
}

std::string HistoryRegistry::Down()
{
    m_current_pos = std::min(m_current_pos + 1, m_history.size() - 1);

    return m_history[m_current_pos];
}
