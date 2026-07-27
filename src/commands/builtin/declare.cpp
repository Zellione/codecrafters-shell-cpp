#include "declare.h"

#include <ranges>
#include <string_view>

using Ast::Command;

DeclareCommand::DeclareCommand(VariableRegistry &registry)
    : BuiltinCommand("declare"), m_variables(registry)
{
}

int DeclareCommand::Process(const Command &comm)
{
    std::string variable_name;
    for (size_t i = 1; i < comm.Args.size(); i++)
    {
        if (comm.Args[i].contains('='))
        {
            std::vector<std::string_view> variable_split =
                (comm.Args[i] | std::views::split('=') |
                 std::views::transform(
                     [](auto &&sub) { return std::string_view(sub); })) |
                std::ranges::to<std::vector>();

            if (variable_split.size() == 2)
            {
                std::string key = std::string(variable_split[0]);
                std::string value = std::string(variable_split[1]);

                if (!IsValidVariableName(key))
                {
                    std::cerr << std::format(
                        "declare: `{}': not a valid identifier\n",
                        comm.Args[i]);
                    return 1;
                }

                m_variables.AddVariable(key, value);
            }

            return 0;
        }

        if (comm.Args[i] == "-p" && (i + 1) < comm.Args.size())
        {
            variable_name = comm.Args[++i];

            break;
        }
    }

    std::string variable_value = m_variables.GetVariable(variable_name);
    if (!variable_name.empty() && !variable_value.empty())
    {
        std::cout << std::format("declare -- {}=\"{}\"\n", variable_name,
                                 variable_value);
        return 0;
    }

    std::cerr << std::format("declare: {}: not found\n", variable_name);

    return 1;
}

bool DeclareCommand::IsValidVariableName(const std::string &name)
{
    for (int i = 0; i < name.length(); i++)
    {
        if ((name[i] >= 'A' && name[i] <= 'Z') ||
            (name[i] >= 'a' && name[i] <= 'z') || name[i] == '_' ||
            (name[i] >= '0' && name[i] <= '9'))
        {
            if (i == 0 && (name[i] >= '0' && name[i] <= '9'))
            {
                return false;
            }

            continue;
        }

        return false;
    }

    return true;
}
