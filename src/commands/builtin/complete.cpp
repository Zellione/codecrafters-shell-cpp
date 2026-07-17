#include "complete.h"

#include <format>

using Ast::Command;

int CompleteCommand::Process(const Ast::Command &comm) const
{
    std::string name;
    std::string completion;
    bool flag_print = false;
    bool flag_create = false;
    bool flag_remove = false;

    for (auto it = comm.Args.begin(); it != comm.Args.end(); it++)
    {
        const std::string &arg = *it;
        // printFlag
        if (arg == "-p")
        {
            flag_print = true;
            continue;
        }

        // addFlag
        if (arg == "-C")
        {
            if ((it + 1) != comm.Args.end())
            {
                completion = *(it + 1);
            }
            flag_create = true;
            continue;
        }

        // removeFlag
        if (arg == "-r")
        {
            flag_remove = true;
            continue;
        }

        name = arg;
    }

    if (flag_remove)
    {
        if (m_completeRegistry->Has(name))
        {
            m_completeRegistry->Remove(name);
        }
    }

    if (flag_print)
    {
        Print(comm, name);
        return 0;
    }

    if (flag_create)
    {
        Create(name, completion);
    }

    return 0;
}

void CompleteCommand::Print(const Command &comm, const std::string &name) const
{
    if (!m_completeRegistry->Has(name))
    {
        std::cerr << std::format("complete: {}: no completion specification\n",
                                 name);

        return;
    }

    std::cout << std::format("complete -C '{}' {}\n",
                             m_completeRegistry->Get(name), name);
}

void CompleteCommand::Create(const std::string &name,
                             const std::string &completion) const
{
    if (m_completeRegistry->Has(name))
    {
        return;
    }

    m_completeRegistry->Add(name, completion);
}

CompleteCommand::CompleteCommand(CompleteRegistry *completeRegistry)
    : BuiltinCommand("complete", "complete is a shell builtin"),
      m_completeRegistry(completeRegistry)
{
}
