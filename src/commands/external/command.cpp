#include "command.h"
#include "../../helper.h"
#include "../../shell.h"

#include <algorithm>
#include <array>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>

using Ast::Command;
using std::string;
using std::vector;

ExternalCommand::ExternalCommand(Output *output) : m_output(output) {}

int ExternalCommand::Exec(const Command &command,
                          const vector<char *> &env_vars)
{
    if (command.Args.empty())
    {
        return 1;
    }

    std::filesystem::path exec_path = find_executable(command.Args[0]);
    if (exec_path.empty())
    {
        return 1;
    }

    vector<char *> argv;
    FillArgV(command, argv);

    if (!env_vars.empty())
    {
        execvpe(argv[0], argv.data(), env_vars.data());

        for (char *env_var : env_vars)
        {
            free(env_var);
        }
    }
    else
    {
        execvp(argv[0], argv.data());
    }

    return -1;
}

void ExternalCommand::ReadPipes(int stdout_fd, int stderr_fd, CmdResult &result)
{
    // register both pipe read ends with poll
    std::array<struct pollfd, 2> fds;
    fds[0].fd = stdout_fd;
    fds[0].events = POLLIN;
    fds[1].fd = stderr_fd;
    fds[1].events = POLLIN;

    std::array<char, 256> buffer{};

    auto drain = [&](int fd, string &out) {
        ssize_t n;
        while ((n = read(fd, buffer.begin(), buffer.size())) > 0)
        {
            out.append(buffer.begin(), n);
        }
    };

    while (true)
    {
        // Shell::ReapBackgroundJobs();

        fds[0].revents = 0;
        fds[1].revents = 0;

        // wait until at least one fd is ready (no timeout: -1)
        int ready = poll(fds.data(), fds.size(), 100);
        if (ready < 0)
        {
            if (errno == EINTR)
            {
                continue; // interrupted, just continue
            }
            break; // real error
        }

        if (ready == 0)
        {
            continue;
        }

        // check stdout
        if (fds[0].fd != -1 && ((fds[0].revents & POLLIN) != 0))
        {
            ssize_t n = read(stdout_fd, buffer.begin(), buffer.size());
            if (n > 0)
            {
                result.stdout_output.append(buffer.data(), n);
            }
        }
        if (fds[0].fd != -1 && ((fds[0].revents & (POLLHUP | POLLERR)) != 0))
        {
            drain(stdout_fd, result.stdout_output);
            fds[0].fd = -1;
        }

        // check stderr
        if (fds[1].fd != -1 && ((fds[1].revents & POLLIN) != 0))
        {
            ssize_t n = read(stderr_fd, buffer.begin(), sizeof(buffer.data()));
            if (n > 0)
            {
                result.stderr_output.append(buffer.begin(), n);
            }
        }
        if (fds[1].fd != -1 && ((fds[1].revents & (POLLHUP | POLLERR)) != 0))
        {
            drain(stderr_fd, result.stderr_output);
            fds[1].fd = -1;
        }

        // exit when both pipes are closed
        if (fds[0].fd == -1 && fds[1].fd == -1)
        {
            break;
        }
    }

    close(stdout_fd);
    close(stderr_fd);
}

vector<string> ExternalCommand::SearchExecutable(const string &partial)
{
    vector<string> commands;
    vector<std::filesystem::path> folders = get_valid_path_folders();

    for (const auto &folder : folders)
    {
        for (const auto &entry : std::filesystem::directory_iterator(folder))
        {
            string command = entry.path().filename().string();
            if (std::filesystem::is_regular_file(entry.status()) &&
                command.starts_with(partial) &&
                std::ranges::find(commands, command) == commands.end())
            {
                commands.push_back(command);
            }
        }
    }

    return commands;
}

void ExternalCommand::FillArgV(const Command &command, vector<char *> &out_argv)
{
    // Build argv
    for (const auto &arg : command.Args)
    {
        out_argv.push_back(const_cast<char *>(arg.c_str()));
    }
    out_argv.push_back(nullptr);
}

void ExternalCommand::ExecCommand(const Ast::Command &command) const
{
    vector<char *> argv;
    FillArgV(command, argv);

    execvp(argv[0], argv.data());
}
