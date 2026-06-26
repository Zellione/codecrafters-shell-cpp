#include "exec_external_comm.h"
#include "helper.h"

#include <filesystem>
#include <iomanip>
#include <sstream>
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
    int stdout_pipe[2];
    int stderr_pipe[2];
    pipe(stdout_pipe);
    pipe(stderr_pipe);

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

    auto read_pipe = [](int fd) {
        std::string out;
        char buffer[256];
        ssize_t n;
        while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
            out.append(buffer, n);
        }
        close(fd);

        return out;
    };

    m_Output->Put(tokens, read_pipe(stdout_pipe[0]), OutputTarget::STDOUT);
    m_Output->Put(tokens, read_pipe(stderr_pipe[0]), OutputTarget::ERROUT);

    int status;
    waitpid(pid, &status, 0);

    int exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    return true;
}
