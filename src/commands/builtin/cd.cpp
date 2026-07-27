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
    std::string path(comm.Args[1]);

    fs::path new_path(path);
    fs::path new_absolute_path;
    try
    {
        new_absolute_path = fs::canonical(m_current_directory / new_path);
    }
    catch (const std::filesystem::filesystem_error &err)
    {
        new_absolute_path = new_path;
    }
    if (!fs::exists(new_path) && !fs::exists(new_absolute_path))
    {
        std::cerr << std::format("cd: {}: No such file or directory\n",
                                 comm.Args[1]);
        return 1;
    }

    if (new_path.is_absolute())
    {
        m_current_directory = fs::canonical(new_path);
    }
    else
    {
        m_current_directory = fs::canonical(m_current_directory / new_path);
    }

    return 0;
}
