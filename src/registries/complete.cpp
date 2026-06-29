#include "complete.h"

void CompleteRegistry::Add(const std::string &name,
                           const std::string &completion)
{
    m_completions.emplace(name, completion);
}

const std::string &CompleteRegistry::Get(const std::string &name) const
{
    return m_completions.find(name)->second;
}

bool CompleteRegistry::Has(const std::string &name) const
{
    return m_completions.contains(name);
}
