#include "executor.h"

#include "../commands/error/err_not_found.h"

#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

using Ast::Command;
using Ast::Node;
using Ast::NodeType;
using Ast::Pipeline;
using Ast::Sequence;
using std::array;
using std::string;
using std::vector;

Executor::Executor(Output &output, BuiltinRegistry &builtin_registry,
                   ExternalCommand &ext_comm)
    : m_output(output), m_builtin_registry(builtin_registry),
      m_external_comm(ext_comm)
{
}

int Executor::Exec(Node *sequence, const vector<char *> &env_vars,
                   CmdResult *result)
{
    Node *next_node = sequence;
    int status = 0;
    NodeType current_type;
    bool background = HasBackgroundFlag(sequence);

    if (background)
    {
        return ExecBackground(next_node, env_vars);
    }

    if (next_node == nullptr)
    {
        ErrorNotFound::Raise({});

        return 1;
    }

    while (next_node != nullptr)
    {
        current_type = next_node->Type;
        switch (next_node->Type)
        {
        case NodeType::SEQUENCE:
            status = ExecSequence(&next_node, env_vars);
            break;
        case NodeType::PIPELINE:
            status = ExecPipeline(next_node, env_vars, false, result);
            break;
        }

        if (status != 0 || (next_node->Type == NodeType::PIPELINE &&
                            current_type == NodeType::PIPELINE))
        {
            return status;
        }
    }

    return status;
}

int Executor::ExecSequence(Node **sequence, const vector<char *> &env_vars)
{
    int status = 0;
    Sequence *seq = (*sequence)->AstNode->SequenceNode;
    if (seq->Left->Type == NodeType::PIPELINE)
    {
        status = ExecPipeline(seq->Left, {});
    }

    *sequence = seq->Right;
    if ((*sequence) != nullptr && (*sequence)->Type == NodeType::SEQUENCE)
    {
        return ExecSequence(sequence, env_vars);
    }

    return status;
}

int Executor::ExecPipeline(Node *pipeline, const vector<char *> &env_vars,
                           bool exec_last_in_place, CmdResult *result)
{
    Pipeline *current_pipeline = pipeline->AstNode->PipelineNode;

    std::vector<pid_t> pids;
    int prev_pipe_fd = -1;
    std::array<int, 2> stdout_pipe{-1, -1};
    std::array<int, 2> stderr_pipe{-1, -1};

    int flags_stdout = fcntl(stdout_pipe[0], F_GETFL, 0);
    int flags_stderr = fcntl(stderr_pipe[0], F_GETFL, 0);
    fcntl(stdout_pipe[0], F_SETFL, flags_stdout | O_NONBLOCK);
    fcntl(stderr_pipe[0], F_SETFL, flags_stderr | O_NONBLOCK);

    // Create pipes for final output capture
    if (pipe(stdout_pipe.data()) == -1 || pipe(stderr_pipe.data()) == -1)
    {
        return 1;
    }

    for (auto it = current_pipeline->Commands.begin();
         it != current_pipeline->Commands.end(); it++)
    {
        const std::string &command = current_pipeline->Commands[0].Args[0];
        if (command == "exit" || command == "jobs" || command == "complete" ||
            command == "history")
        {
            return ExecCommand(*it, env_vars);
        }

        bool is_last = (it + 1) == current_pipeline->Commands.end();
        if (is_last && exec_last_in_place)
        {
            if ((it + 1) != current_pipeline->Commands.end())
            {
                dup2(prev_pipe_fd, STDIN_FILENO);
                close(prev_pipe_fd);
            }

            int status = ExecCommand(*it, env_vars);
            _exit(status);
        }

        std::array<int, 2> pipe_fd = {-1, -1};

        // Create pipes if this is not the last command
        if ((it + 1) != current_pipeline->Commands.end())
        {
            pipe(pipe_fd.data());
        }

        pid_t pid = fork();

        if (pid == 0)
        {
            // CHILD PROCESS

            // 1. Setup Input: If not first command, read from previous input
            if (it != current_pipeline->Commands.begin() && prev_pipe_fd >= 0)
            {
                dup2(prev_pipe_fd, STDIN_FILENO);
                close(prev_pipe_fd);
            }

            // 2. Setup Output: If not last command, write to current pipe
            if ((it + 1) != current_pipeline->Commands.end())
            {
                // Intermediate command
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
                close(stdout_pipe[0]); // close read end of stdout pipe
                close(stdout_pipe[1]); // close write end of stdout pipe
                close(stderr_pipe[0]); // close read end of stderr pipe
                close(stderr_pipe[1]); // close write end of stderr pipe
            }
            else
            {
                // Last command
                dup2(stdout_pipe[1], STDOUT_FILENO);
                dup2(stderr_pipe[1], STDERR_FILENO);
                close(stdout_pipe[0]); // close read end of stdout pipe
                close(stdout_pipe[1]); // close write end of stdout pipe
                close(stderr_pipe[0]); // close read end of stderr pipe
                close(stderr_pipe[1]); // close write end of stderr pipe
            }

            // 3. close any lingering previous pipefd
            if (it != current_pipeline->Commands.begin())
            {
                close(prev_pipe_fd);
            }

            // 4. exec commmand
            int status = ExecCommand(*it, env_vars);

            // Only reached on error
            _exit(status);
        }
        else
        {
            // PARENT PROCESS
            pids.push_back(pid);

            if ((it + 1) != current_pipeline->Commands.end())
            {
                close(pipe_fd[1]);

                // The read end of the current pipe becomes the previous
                prev_pipe_fd = pipe_fd[0];
            }
            else
            {
                // Close the write end of the current pipe immediatly in parent
                close(stdout_pipe[1]);
                close(stderr_pipe[1]);
            }
        }
    }

    std::string out;
    std::string err;
    fd_set readfds;
    struct timeval tv;
    bool is_first_iteration = true;
    while (true)
    {
        bool still_running =
            ReadPipes(stdout_pipe[0], stderr_pipe[0], readfds, out, err, tv);

        if (result == nullptr)
        {
            if (!out.empty() || is_first_iteration)
            {
                m_output.Put(current_pipeline->Commands.back(), out,
                             OutputTarget::STDOUT);
            }
            if (!err.empty() || is_first_iteration)
            {
                m_output.Put(current_pipeline->Commands.back(), err,
                             OutputTarget::STDERR);
            }
        }
        else
        {
            result->Fill(
                {.stdout_output = out, .stderr_output = err, .exit_code = 1});
        }
        is_first_iteration = false;
        if (!still_running)
        {
            if (stdout_pipe[0] != -1)
            {
                close(stdout_pipe[0]);
            }
            if (stderr_pipe[0] != -1)
            {
                close(stderr_pipe[0]);
            }
            break;
        }
        out.clear();
        err.clear();

        ReapJobs();
        ReapBackgroundJobs();
    }

    // wait for all children
    pid_t last_pid = pids.back();
    int status;
    waitpid(last_pid, &status, 0);
    status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;

    if (result != nullptr)
    {
        result->exit_code = status;
    }
    for (pid_t pid : pids)
    {
        if (pid == last_pid)
        {
            continue;
        }

        int reap_status;
        waitpid(pid, &reap_status, WNOHANG);

        if (reap_status == 0)
        {
            m_jobs_registry.Add({.pid = pid,
                                 .read_fd_out = -1,
                                 .read_fd_err = -1,
                                 .status = BackgroundJobStatus::RUNNING,
                                 .commandline = "pipeline status"});
        }
        else if (reap_status == -1)
        {
            perror("waitpid");
        }
    }

    // Parent must close final read pipe and err pipe
    if (prev_pipe_fd != -1)
    {
        close(prev_pipe_fd);
    }

    return status;
}

int Executor::ExecCommand(const Command &comm, const vector<char *> &env_vars)
{
    const BuiltinCommand *builtin_comm = m_builtin_registry.FindCommand(comm);
    if (builtin_comm != nullptr)
    {
        return builtin_comm->Execute(comm);
    }

    int status = m_external_comm.Exec(comm, env_vars);

    if (status != 0)
    {
        ErrorNotFound::Raise(comm);
    }

    return status;
}

bool Executor::HasBackgroundFlag(Node *nodes)
{
    bool has_bg_flag = false;
    Node *next_node = nodes;
    if (next_node == nullptr)
    {
        return has_bg_flag;
    }

    switch (next_node->Type)
    {
    case NodeType::SEQUENCE:
        has_bg_flag =
            HasBackgroundFlag(next_node->AstNode->SequenceNode->Left) ||
            HasBackgroundFlag(next_node->AstNode->SequenceNode->Right);
        break;
    case NodeType::PIPELINE:
        for (const auto &command : next_node->AstNode->PipelineNode->Commands)
        {
            has_bg_flag = has_bg_flag || command.Background;
        }
        break;
    }

    return has_bg_flag;
}

bool Executor::ReadPipes(int stdout_fd, int stderr_fd, fd_set &readfds,
                         std::string &out, std::string &err, struct timeval &tv)
{
    FD_ZERO(&readfds);
    if (stdout_fd != -1)
    {
        FD_SET(stdout_fd, &readfds);
    }
    if (stderr_fd != -1)
    {
        FD_SET(stderr_fd, &readfds);
    }
    int maxfd = std::max(stdout_fd, stderr_fd);

    // Add a timeout
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    // Wait for data with a timeout
    int activity = select(maxfd + 1, &readfds, nullptr, nullptr, &tv);

    if (activity < 0)
    {
        // interrupted by signal, continue
        if (errno == EINTR)
        {
            return true;
        }

        perror("select");
        return false;
    }

    // timeout occured
    if (activity == 0)
    {
        return true;
    }

    bool stdout_open = true;
    bool stderr_open = true;

    if (stdout_fd != -1 && FD_ISSET(stdout_fd, &readfds))
    {
        std::array<char, 4096> buffer;
        ssize_t count = read(stdout_fd, buffer.data(), buffer.size() - 1);

        if (count > 0)
        {
            out.append(buffer.data(), count);
        }
        else if (count == 0)
        {
            stdout_open = false;
        }
        else
        {
            perror("read stdout");
            stdout_open = false;
        }
    }

    if (stderr_fd != -1 && FD_ISSET(stderr_fd, &readfds))
    {
        std::array<char, 4096> buffer;
        ssize_t count = read(stderr_fd, buffer.data(), buffer.size() - 1);

        if (count > 0)
        {
            err.append(buffer.data(), count);
        }
        else if (count == 0)
        {
            stderr_open = false;
        }
        else
        {
            perror("read stderr");
            stderr_open = false;
        }
    }

    return stdout_open && stderr_open;
}

void Executor::ReapJobs()
{
    std::map<unsigned int, BackgroundJob> &jobs = m_jobs_registry.GetAll();

    for (auto it = jobs.begin(); it != jobs.end();)
    {
        BackgroundJob &job = it->second;

        int status;
        pid_t result = waitpid(job.pid, &status, WNOHANG);

        if (result == 0 || result == -1)
        {
            it++;
            continue;
        }

        if (WIFEXITED(status))
        {
            it = jobs.erase(it);
        }
    }
}

int Executor::ExecBackground(Ast::Node *commandchain,
                             const std::vector<char *> &env_vars)
{
    std::array<int, 2> stdout_pipe;
    std::array<int, 2> stderr_pipe;
    pipe(stdout_pipe.data());
    pipe(stderr_pipe.data());

    int flags_stdout = fcntl(stdout_pipe[0], F_GETFL, 0);
    int flags_stderr = fcntl(stderr_pipe[0], F_GETFL, 0);
    fcntl(stdout_pipe[0], F_SETFL, flags_stdout | O_NONBLOCK);
    fcntl(stderr_pipe[0], F_SETFL, flags_stderr | O_NONBLOCK);

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

        NodeType current_type;
        Node *next_node = commandchain;
        int status = 0;
        while (next_node != nullptr)
        {
            current_type = next_node->Type;
            switch (next_node->Type)
            {
            case NodeType::SEQUENCE:
                status = ExecSequence(&next_node, env_vars);
                break;
            case NodeType::PIPELINE:
                status = ExecPipeline(next_node, env_vars, true);
                break;
            }

            if (status != 0)
            {
                _exit(status);
            }

            if ((next_node->Type == NodeType::PIPELINE &&
                 current_type == NodeType::PIPELINE))
            {
                break;
            }
        }

        _exit(127);
    }

    // Parent process
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    const Command &last_command = Ast::get_last_command(commandchain);
    unsigned int job_number = m_background_jobs.Add(
        {.pid = pid,
         .read_fd_out = stdout_pipe[0],
         .read_fd_err = stderr_pipe[0],
         .status = BackgroundJobStatus::RUNNING,
         .commandline = Ast::stringify_command(last_command)});
    m_output.Put({}, std::format("[{}] {}\n", job_number, pid),
                 OutputTarget::STDOUT);

    return 0;
}

int Executor::ReapBackgroundJobs()
{
    std::map<unsigned int, BackgroundJob> &background_jobs =
        m_background_jobs.GetAll();

    for (auto it = background_jobs.begin(); it != background_jobs.end();)
    {
        BackgroundJob &background_job = it->second;
        if (background_job.status == BackgroundJobStatus::DONE)
        {
            it++;
            continue;
        }

        int status;
        pid_t result = waitpid(background_job.pid, &status, WNOHANG);

        if (result == 0)
        {
            it++;
            continue;
        }

        string output;
        string error;

        std::array<char, 4096> buffer;
        ssize_t n;

        while ((n = read(background_job.read_fd_out, buffer.begin(),
                         sizeof(buffer.begin()))) > 0)
        {
            output.append(buffer.begin(), n);
        }
        close(background_job.read_fd_out);
        while ((n = read(background_job.read_fd_err, buffer.begin(),
                         sizeof(buffer.begin()))) > 0)
        {
            error.append(buffer.begin(), n);
        }
        close(background_job.read_fd_err);

        if (WIFEXITED(status))
        {
            background_job.status = BackgroundJobStatus::DONE;
            background_job.commandline = background_job.commandline.substr(
                0, background_job.commandline.length() - 2);
        }

        m_output.Put({}, output, OutputTarget::STDOUT);
        m_output.Put({}, error, OutputTarget::STDERR);
    }

    return 0;
}

void Executor::PrintDoneBGJobs() { m_background_jobs.PrintDone(m_output); }

JobsRegistry &Executor::GetBGJobsRegistry() { return m_background_jobs; }
