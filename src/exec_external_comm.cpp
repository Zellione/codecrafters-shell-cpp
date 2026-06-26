#include "exec_external_comm.h"
#include "helper.h"

#include <filesystem>
#include <iomanip>
#include <poll.h>
#include <sstream>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>

ExecExternalCommand::ExecExternalCommand(Output *output) : m_Output(output) {}

bool ExecExternalCommand::Exec(const std::string &commandline) const {
    std::vector<Token> tokens = TokenParser::Parse(commandline);
    if (tokens.empty()) {
        return false;
    }

    std::filesystem::path exec_path = find_executable(tokens[0].token);
    if (exec_path == "") {
        return false;
    }

    std::stringstream exec_path_with_args;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type != TokenType::NORMAL) {
            break;
        }
        if (tokens[i].token.contains(' ') || tokens[i].token.contains('\'') ||
            tokens[i].token.contains('\\')) {
            exec_path_with_args << std::quoted(tokens[i].token);
        } else {
            exec_path_with_args << tokens[i].token;
        }
        if (i != tokens.size() - 1) {
            exec_path_with_args << " ";
        }
    }

    // TODO: Change this to a more complex bevahiour, which will be able to
    // catch stdout and stderr seperately
    std::array<int, 2> stdout_pipe;
    std::array<int, 2> stderr_pipe;
    pipe(stdout_pipe.data());
    pipe(stderr_pipe.data());

    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        // Build argv
        std::vector<char *> argv;
        for (auto &token : tokens) {
            if (token.type == TokenType::NORMAL) {
                argv.push_back(const_cast<char *>(token.token.c_str()));
            }
        }
        argv.push_back(nullptr);

        execvp(argv[0], argv.data());
        _exit(127); // exec failed
    }

    // Parent process
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    CmdResult result;
    ReadPipes(stdout_pipe[0], stderr_pipe[0], result);

    m_Output->Put(tokens, result.stdout_output, OutputTarget::STDOUT);
    m_Output->Put(tokens, result.stderr_output, OutputTarget::STDERR);

    int status;
    waitpid(pid, &status, 0);

    result.exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    return true;
}

void ExecExternalCommand::ReadPipes(int stdout_fd, int stderr_fd,
                                    CmdResult &result) {
    // register both pipe read ends with poll
    std::array<struct pollfd, 2> fds;
    fds[0].fd = stdout_fd;
    fds[0].events = POLLIN;
    fds[1].fd = stderr_fd;
    fds[1].events = POLLIN;

    std::array<char, 256> buffer;

    while (true) {
        // wait until at least one fd is ready (no timeout: -1)
        int ready = poll(fds.data(), fds.size(), -1);
        if (ready <= 0) {
            break;
        }

        // check stdout
        if ((fds[0].revents & POLLIN) != 0) {
            ssize_t n = read(stdout_fd, buffer.data(), sizeof(buffer.data()));
            if (n > 0) {
                result.stdout_output.append(buffer.data(), n);
            }
        }
        if ((fds[0].revents & POLLHUP) != 0) {
            fds[0].fd = -1;
        }

        // check stderr
        if ((fds[1].revents & POLLIN) != 0) {
            ssize_t n = read(stderr_fd, buffer.data(), sizeof(buffer.data()));
            if (n > 0) {
                result.stderr_output.append(buffer.data(), n);
            }
        }
        if ((fds[1].revents & POLLHUP) != 0) {
            fds[1].fd = -1;
        }

        // exit when both pipes are closed
        if (fds[0].fd == -1 && fds[1].fd == -1) {
            break;
        }
    }

    close(stdout_fd);
    close(stderr_fd);
}
