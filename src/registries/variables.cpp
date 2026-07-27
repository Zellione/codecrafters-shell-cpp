#include "variables.h"
#include <stdexcept>

VariableRegistry::VariableRegistry()
{
    m_variables.emplace("HOME", std::getenv("HOME"));
}

void VariableRegistry::AddVariable(const std::string &key,
                                   const std::string &value)
{
    if (m_variables.contains(key))
    {
        m_variables.erase(key);
    }

    m_variables.emplace(key, value);
}

std::string VariableRegistry::GetVariable(const std::string &key) const
{
    try
    {
        return m_variables.at(key);
    }
    catch (const std::out_of_range &)
    {
        return "";
    }
}
