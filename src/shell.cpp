#include "shell.h"
#include "commands/builtin/complete.h"
#include "commands/builtin/echo.h"
#include "commands/builtin/exit.h"
#include "commands/builtin/jobs.h"
#include "commands/builtin/type.h"
#include "commands/error/err_not_found.h"
#include "helper/filesystem.h"
#include "output/console_error.h"
#include "output/console_output.h"
#include "output/redirect_stderr.h"
#include "output/redirect_stdout.h"
#include "readline/readline.h"
#include "registries/complete.h"

#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using std::string;
using std::vector;

Shell::Shell()
    : m_external_comm(&m_output, m_jobs_registry),
      m_jobs_registry(new JobsRegistry()),
      m_complete_registry(new CompleteRegistry(&m_external_comm)),
      m_line_ready(false), m_exit_shell(false), m_main_process(true)
{
    m_output.AddType(new RedirectStdOut());
    m_output.AddType(new RedirectStdErr());
    m_output.AddType(new ConsoleOutput());
    m_output.AddType(new ConsoleError());

    m_registry.RegisterCommand(new EchoCommand(&m_output));
    m_registry.RegisterCommand(new ExitCommand(&m_output));
    m_registry.RegisterCommand(new TypeCommand(&m_registry, &m_output));
    m_registry.RegisterCommand(new JobsCommand(&m_output, *m_jobs_registry));
    m_registry.RegisterCommand(
        new CompleteCommand(&m_output, m_complete_registry));
}

Shell::~Shell()
{
    delete m_complete_registry;
    m_complete_registry = nullptr;

    delete m_jobs_registry;
    m_jobs_registry = nullptr;
}

void Shell::run()
{
    while (!m_exit_shell)
    {
        rl_callback_handler_install("$ ", Shell::LineHandler);
        while (!m_line_ready)
        {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);

            struct timeval timeout{.tv_sec = 0, .tv_usec = 100000}; // 100ms
            int ret =
                select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &timeout);

            if (ret > 0 && FD_ISSET(STDIN_FILENO, &fds))
            {
                rl_callback_read_char(); // feeds readline one keystroke at a
                                         // time
            }

            ReapBackgroundJobs();
        }

        vector<Token> tokens = TokenParser::Parse(m_current_input);
        vector<vector<Token>> commands = SplitCommandChain(tokens);
        int status = -1;
        if (HasBackgroundFlag(tokens))
        {
            status = ExecuteBackgroundCommandChain(commands);
        }
        else
        {
            status = ExecuteCommandChain(commands);
        }

        if (status != 0)
        {
            ErrorNotFound::Raise(tokens);
        }

        m_last_prompt = m_current_input;
        m_current_input.clear();
        m_line_ready = false;
    }

    rl_callback_handler_remove();
}

int Shell::TabAutoComplete(int count, int key)
{
    Shell &shell = Shell::Instance();

    if (rl_line_buffer == shell.m_last_prompt &&
        shell.m_autocomplete.size() > 1)
    {
        return Shell::TabAutoCompleteMulti(count, key);
    }
    vector<Token> tokens = TokenParser::Parse(rl_line_buffer);

    shell.m_autocomplete = shell.CollectAutocompletes(rl_line_buffer, tokens);
    shell.m_last_prompt = rl_line_buffer;

    if (shell.m_autocomplete.empty())
    {
        std::cout << '\x07';
        return 1;
    }

    std::string newprompt = std::string(rl_line_buffer);
    size_t pos = newprompt.rfind(' ');
    if (pos == std::string::npos)
    {
        pos = 0;
    }
    else
    {
        pos++;
    }

    const Token &token = tokens.back();
    std::string cleaned_token = token.token;
    if (cleaned_token.contains('/'))
    {
        cleaned_token = cleaned_token.substr(
            cleaned_token.rfind('/') + 1,
            cleaned_token.length() -
                (cleaned_token.length() - cleaned_token.rfind('/')));
        pos = newprompt.rfind('/') + 1;
    }

    if (shell.m_autocomplete.size() == 1)
    {
        newprompt = newprompt.replace(pos, newprompt.length() - pos,
                                      shell.m_autocomplete[0].ends_with('/')
                                          ? shell.m_autocomplete[0]
                                          : shell.m_autocomplete[0] + " ");
        rl_replace_line(newprompt.c_str(), 0);
        rl_point = rl_end;

        return 0;
    }

    std::cout << '\x07';

    if (std::string(rl_line_buffer).back() != ' ')
    {
        newprompt = newprompt.replace(pos, newprompt.length() - pos,
                                      shell.LongestCommonPrefix(cleaned_token));
        rl_replace_line(newprompt.c_str(), 0);
        rl_point = rl_end;
    }

    return 0;
}

int Shell::TabAutoCompleteMulti(int count, int key)
{
    Shell &shell = Shell::Instance();

    vector<Token> tokens{};
    if (shell.m_autocomplete.size() > 1)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < shell.m_autocomplete.size(); i++)
        {
            if (i > 0)
            {
                oss << "  ";
            }

            oss << shell.m_autocomplete[i];
        }
        std::string autocompletes =
            std::format("\n{}\n", std::string(oss.str()));

        shell.m_output.Put(tokens, autocompletes, OutputTarget::STDOUT);
        rl_redraw_prompt_last_line();
    }
    return 0;
}

vector<std::string> Shell::CollectAutocompletes(const std::string &partial,
                                                const vector<Token> &tokens)
{
    vector<std::string> autocompletes;

    if (tokens.empty())
    {
        return autocompletes;
    }

    const Token &token = tokens.back();
    std::string current_token = token.token;
    bool is_command = token.type == TokenType::COMMAND && partial.back() != ' ';

    if (current_token.starts_with(m_last_prompt) && partial.back() != ' ')
    {
        std::erase_if(m_autocomplete,
                      [&current_token](const std::string &option) {
                          return !option.starts_with(current_token);
                      });
    }

    if (is_command)
    {
        // Collect commands
        autocompletes = CollectAutocompleteBuiltin(current_token);
        vector<std::string> ext_commands =
            CollectAutocompleteInPath(current_token);
        autocompletes.insert(autocompletes.end(), ext_commands.begin(),
                             ext_commands.end());
    }
    else
    {
        // collect complete builtin autocomplete
        autocompletes = m_complete_registry->Autocomplete(tokens, partial);

        if (autocompletes.empty())
        {
            // collect files
            vector<std::string> files =
                CollectAutocompleteInDir(partial, tokens);
            autocompletes.insert(autocompletes.end(), files.begin(),
                                 files.end());
        }
    }

    // Sort commands
    std::ranges::sort(autocompletes);
    autocompletes.erase(
        std::unique(autocompletes.begin(), autocompletes.end()), // NOLINT
        autocompletes.end());

    return autocompletes;
}

vector<std::string> Shell::CollectAutocompleteInPath(const std::string &partial)
{
    return ExternalCommand::SearchExecutable(partial);
}

vector<std::string>
Shell::CollectAutocompleteBuiltin(const std::string &partial) const
{
    return m_registry.AutoComplete(partial);
}

vector<std::string> Shell::CollectAutocompleteInDir(const std::string &partial,
                                                    const vector<Token> &tokens)
{
    const Token &token = tokens.back();
    std::string pwd = get_current_dir_name();
    std::string relative_path = token.token;

    if (token.token.ends_with('/'))
    {
        relative_path.pop_back();
    }

    if (token.token.contains('/'))
    {
        pwd = std::format("{}/{}", pwd,
                          token.token.substr(0, token.token.length() -
                                                    (token.token.length() -
                                                     token.token.rfind('/'))));
        relative_path =
            token.token.substr(token.token.rfind('/') + 1,
                               token.token.length() - (token.token.length() -
                                                       token.token.rfind('/')));
    }

    vector<std::string> files = get_files_from_dir(pwd);
    vector<std::string> dirs = get_subdirs_from_dir(pwd);
    files.insert(files.end(), dirs.begin(), dirs.end());
    if (partial.back() != ' ')
    {
        std::erase_if(files, [&relative_path](const std::string &file) {
            return !file.starts_with(relative_path);
        });
    }

    return files;
}

std::string Shell::LongestCommonPrefix(const std::string &partial) const
{
    std::string token = partial;
    size_t seperator_pos = token.rfind('/');
    if (seperator_pos != std::string::npos)
    {
        token = token.substr(seperator_pos, token.length() - seperator_pos);
    }
    std::string autocomplete = token;
    size_t max_length =
        std::ranges::max_element(m_autocomplete, {}, &std::string::size)
            ->length();
    for (size_t pos = autocomplete.length(); pos < max_length; pos++)
    {
        char c = 0;
        bool outer_break = false;
        for (const auto &command : m_autocomplete)
        {
            if (command.length() <= pos)
            {
                return autocomplete;
            }
            if (c == 0)
            {
                c = command[pos];
                continue;
            }

            if (command[pos] != c)
            {
                return autocomplete;
            }
        }

        autocomplete += c;
    }

    return autocomplete;
}

int Shell::ReapBackgroundJobs()
{
    Shell &shell = Shell::Instance();
    if (!shell.m_main_process)
    {
        return 0;
    }

    std::map<unsigned int, BackgroundJob> &background_jobs =
        shell.m_jobs_registry->GetAll();

    for (auto it = background_jobs.begin(); it != background_jobs.end();)
    {
        BackgroundJob &background_job = it->second;
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

        it = background_jobs.erase(it);

        shell.m_output.Put({}, output, OutputTarget::STDOUT);
        shell.m_output.Put({}, error, OutputTarget::STDERR);
    }

    return 0;
}

void Shell::LineHandler(char *line)
{
    Shell &shell = Shell::Instance();

    if (line != nullptr)
    {
        shell.m_current_input = line;
        free(line);
    }

    shell.m_line_ready = true;
    rl_callback_handler_remove();
}

int Shell::ExecuteCommand(const vector<Token> &command) const
{
    const BuiltinCommand *builtin_command = m_registry.FindCommand(command);
    if (builtin_command != nullptr)
    {
        return builtin_command->Execute(command);
    }

    return m_external_comm.Exec(command, {});
}

int Shell::ExecuteCommandChain(const vector<vector<Token>> &commands) const
{
    if (commands.empty())
    {
        return -1;
    }

    for (const auto &command : commands)
    {
        int status = ExecuteCommand(command);

        if (status != 0)
        {
            return status;
        }
    }

    return 0;
}

int Shell::ExecuteBackgroundCommandChain(const vector<vector<Token>> &commands)
{
    if (commands.empty())
    {
        return -1;
    }

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
        m_main_process = false;

        close(stdout_pipe[0]);
        close(stderr_pipe[0]);

        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stderr_pipe[1], STDERR_FILENO);

        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        for (size_t i = 0; i + 1 < commands.size(); i++)
        {
            int status = ExecuteCommand(commands[i]);
            if (status != 0)
            {
                _exit(status);
            }
        }

        const vector<Token> &last = commands.back();
        m_external_comm.ExecCommand(last);

        _exit(127);
    }

    // Parent process
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);

    unsigned int job_number = m_jobs_registry->Add(
        {.pid = pid,
         .read_fd_out = stdout_pipe[0],
         .read_fd_err = stderr_pipe[0],
         .commandline = std::move(GetCommandline(commands))});
    m_output.Put({}, std::format("[{}] {}\n", job_number, pid),
                 OutputTarget::STDOUT);

    return 0;
}

vector<vector<Token>> Shell::SplitCommandChain(const vector<Token> &tokens)
{
    vector<vector<Token>> commands;

    auto chunks =
        tokens | std::views::chunk_by([](const Token &a, const Token &b) {
            return (a.type != TokenType::LOGIC_AND_COMMANDS) &&
                   (b.type != TokenType::LOGIC_AND_COMMANDS);
        });

    for (const auto &chunk : chunks)
    {
        if (std::ranges::distance(chunk) == 1 &&
            chunk.begin()->type == TokenType::LOGIC_AND_COMMANDS)
        {
            continue;
        }

        vector<Token> command;
        for (const auto &token : chunk)
        {
            if (token.type != TokenType::BACKGROUND_JOB)
            {
                command.push_back(token);
            }
        }
        commands.push_back(std::move(command));
    }

    return commands;
}

bool Shell::HasBackgroundFlag(const vector<Token> &tokens)
{
    return tokens.size() > 1 && tokens.back().type == TokenType::BACKGROUND_JOB;
}

void Shell::ExitShell(bool exit) { m_exit_shell = exit; }

std::string
Shell::GetCommandline(const std::vector<std::vector<Token>> &commands)
{
    std::stringstream ss;
    for (size_t i = 0; i < commands.size(); i++)
    {
        for (size_t j = 0; j < commands[i].size(); j++)
        {
            ss << commands[i][j].token;
            if (j + 1 != commands[i].size())
            {
                ss << " ";
            }
        }
        if (i + 1 != commands.size())
        {
            ss << " && ";
        }
    }
    ss << " &";

    return ss.str();
}
