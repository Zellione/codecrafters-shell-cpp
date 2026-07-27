#include "cd.h"
#include <filesystem>

using Ast::Command;

namespace fs = std::filesystem;

CdCommand::CdCommand(std::string &current_directory)
    : BuiltinCommand("cd"), m_current_directory(current_directory)
{
}

int CdCommand::Process(const Command &comm)
{
    if (comm.Args.size() < 2 || comm.Args[1].empty())
    {
        return 1;
    }

    fs::path new_path(comm.Args[1]);
    if (!fs::exists(new_path))
    {
        std::cerr << std::format("cd: {}: No such file or directory\n",
                                 comm.Args[1]);
        return 1;
    }

    if (new_path.is_absolute())
    {
        m_current_directory = new_path;
    }
    else
    {
        m_current_directory = fs::canonical(m_current_directory / new_path);
    }

    return 0;
}
