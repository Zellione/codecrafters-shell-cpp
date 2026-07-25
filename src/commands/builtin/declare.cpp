#include "declare.h"

#include <ranges>
#include <string_view>

using Ast::Command;

DeclareCommand::DeclareCommand() : BuiltinCommand("declare") {}

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

            if (variable_split.size() > 2)
            {
                m_variables.emplace(std::string(variable_split[0]),
                                    std::string(variable_split[1]));
            }
        }

        if (comm.Args[i] == "-p" && (i + 1) < comm.Args.size())
        {
            variable_name = comm.Args[++i];

            continue;
        }
    }

    std::cout << std::format("declare: {}: not found\n", variable_name);

    return 0;
}
