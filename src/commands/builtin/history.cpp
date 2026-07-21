#include "history.h"
#include "../../shell.h"
#include "command.h"

#include <fstream>
#include <sstream>

using Ast::Command;

HistoryCommand::HistoryCommand(HistoryRegistry *registry)
    : BuiltinCommand("history"), m_registry(registry)
{
}

int HistoryCommand::Process(const Command &comm) const
{
    size_t limit = 0;
    std::string history_file;
    bool read = false;
    bool write = false;

    if (comm.Args.size() > 1)
    {
        for (size_t i = 1; i < comm.Args.size(); i++)
        {
            if ((comm.Args[i] == "-r" || comm.Args[i] == "-w") &&
                i + 1 < comm.Args.size())
            {
                if (comm.Args[i] == "-r")
                {
                    read = true;
                }

                if (comm.Args[i] == "-w")
                {
                    write = true;
                }

                history_file = comm.Args[++i];
                continue;
            }

            std::istringstream iss(comm.Args[i]);
            char remain;

            if (iss >> limit && !(iss >> remain))
            {
                limit = std::stoull(comm.Args[i]);
            }
        }
    }

    if (!history_file.empty())
    {
        if (read)
        {
            AppendFileToHistory(history_file);
        }

        if (write)
        {
            AppendHistoryToFile(history_file);
        }

        return 0;
    }

    const auto &entries = m_registry->Get();
    for (size_t i = limit > 0 ? entries.size() - limit : 0; i < entries.size();
         i++)
    {
        std::cout << std::format("    {}  {}\n", i + 1, entries[i]);
    }

    return 0;
}

void HistoryCommand::AppendFileToHistory(const std::string &filename) const
{
    std::ifstream history_input(filename);
    if (history_input.is_open())
    {
        std::string line;
        while (std::getline(history_input, line))
        {
            m_registry->Add(line);
        }
        history_input.close();
    }
}

void HistoryCommand::AppendHistoryToFile(const std::string &filename) const
{
    std::ofstream history_output(filename, std::ios::app);
    if (history_output.is_open())
    {
        for (const auto &entry : m_registry->Get())
        {
            history_output << entry << '\n';
        }

        history_output.flush();
        history_output.close();
    }
}
