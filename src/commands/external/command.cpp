#include "command.h"
#include "../../helper.h"
#include "../../shell.h"

#include <algorithm>
#include <array>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>

using std::string;
using std::vector;

ExternalCommand::ExternalCommand(Output *output, JobsRegistry *registry)
    : m_output(output), m_registry(registry)
{
}

int ExternalCommand::Exec(const vector<Token> &tokens,
                          const vector<char *> &env_vars,
                          CmdResult *result_out) const
{
    if (tokens.empty())
    {
        return -1;
    }

    std::filesystem::path exec_path = find_executable(tokens[0].token);
    if (exec_path.empty())
    {
        return -1;
    }

    std::array<int, 2> stdout_pipe;
    std::array<int, 2> stderr_pipe;
    pipe(stdout_pipe.data());
    pipe(stderr_pipe.data());

    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        vector<char *> argv;
        FillArgV(tokens, argv);

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

        _exit(127); // exec failed
    }

    // Parent process
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    CmdResult result;
    ReadPipes(stdout_pipe[0], stderr_pipe[0], result);

    if (result_out == nullptr)
    {
        m_output->Put(tokens, result.stdout_output, OutputTarget::STDOUT);
        m_output->Put(tokens, result.stderr_output, OutputTarget::STDERR);
    }
    else
    {
        result_out->Fill(result);
    }

    int status;
    waitpid(pid, &status, 0);
    result.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    return 0;
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
        Shell::ReapBackgroundJobs();

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

void ExternalCommand::FillArgV(const vector<Token> &tokens,
                               vector<char *> &out_argv)
{
    std::array<TokenType, 5> applicableTypes{
        TokenType::TEXT, TokenType::COMMAND, TokenType::FILE_PATH,
        TokenType::DIR_PATH, TokenType::FLAG};

    // Build argv
    for (const auto &token : tokens)
    {
        if (std::ranges::any_of(applicableTypes,
                                [&token](TokenType applicableType) {
                                    return applicableType == token.type;
                                }))
        {
            out_argv.push_back(const_cast<char *>(token.token.c_str()));
        }
    }
    out_argv.push_back(nullptr);
}

void ExternalCommand::ExecCommand(std::vector<Token> command) const
{
    vector<char *> argv;
    FillArgV(command, argv);

    execvp(argv[0], argv.data());
}
