#include "shell.h"
#include "commands/builtin/complete.h"
#include "commands/builtin/echo.h"
#include "commands/builtin/exit.h"
#include "commands/builtin/history.h"
#include "commands/builtin/jobs.h"
#include "commands/builtin/type.h"
#include "helper/filesystem.h"
#include "output/console_error.h"
#include "output/console_output.h"
#include "output/redirect_stderr.h"
#include "output/redirect_stdout.h"
#include "parser/lexer.h"
#include "parser/parser.h"
#include "readline/readline.h"
#include "registries/complete.h"

#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using Ast::Command;
using Ast::Node;
using std::string;
using std::vector;

Shell::Shell()
    : m_external_comm(&m_output),
      m_complete_registry(new CompleteRegistry(m_executor)),
      m_line_ready(false), m_exit_shell(false), m_main_process(true),
      m_executor(m_output, m_registry, m_external_comm),
      m_history_registry(new HistoryRegistry())
{
    m_output.AddType(new RedirectStdOut());
    m_output.AddType(new RedirectStdErr());
    m_output.AddType(new ConsoleOutput());
    m_output.AddType(new ConsoleError());

    m_registry.RegisterCommand(new EchoCommand());
    m_registry.RegisterCommand(new ExitCommand());
    m_registry.RegisterCommand(new TypeCommand(&m_registry));
    m_registry.RegisterCommand(new JobsCommand(m_executor.GetBGJobsRegistry()));
    m_registry.RegisterCommand(new CompleteCommand(m_complete_registry));
    m_registry.RegisterCommand(new HistoryCommand(m_history_registry));
}

Shell::~Shell()
{
    delete m_complete_registry;
    m_complete_registry = nullptr;

    delete m_history_registry;
    m_history_registry = nullptr;
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

        Lexer lexer(m_current_input);
        Parser parser(lexer);
        Ast::Node *node = nullptr;
        if (!m_current_input.empty())
        {
            node = parser.Parse();
        }

        m_executor.Exec(node, {});

        m_last_prompt = m_current_input;
        m_current_input.clear();
        m_line_ready = false;

        m_executor.PrintDoneBGJobs();
        m_executor.ReapJobs();
    }

    rl_callback_handler_remove();
    BuiltinCommand *comm = m_registry.GetCommandByName("history");
    if (comm != nullptr)
    {
        ((HistoryCommand *)comm)->WriteHistoryOnExit();
    }
}

int Shell::TabAutoComplete(int count, int key)
{
    Shell &shell = Shell::Instance();

    if (rl_line_buffer == shell.m_last_prompt &&
        shell.m_autocomplete.size() > 1)
    {
        return Shell::TabAutoCompleteMulti(count, key);
    }
    string cur_line = rl_line_buffer;
    Lexer lexer(cur_line);
    Parser parser(lexer);
    Node *command_line = parser.Parse();

    shell.m_autocomplete =
        shell.CollectAutocompletes(rl_line_buffer, command_line);
    shell.m_last_prompt = rl_line_buffer;

    if (shell.m_autocomplete.empty())
    {
        std::cout << '\x07';
        return 1;
    }

    string newprompt = string(rl_line_buffer);
    size_t pos = newprompt.rfind(' ');
    if (pos == string::npos)
    {
        pos = 0;
    }
    else
    {
        pos++;
    }

    const Command &command = Ast::get_last_command(command_line);
    string cleaned_token = command.Args.back();
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

    if (string(rl_line_buffer).back() != ' ')
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

        shell.m_output.Put({}, autocompletes, OutputTarget::STDOUT);
        rl_redraw_prompt_last_line();
    }
    return 0;
}

vector<string> Shell::CollectAutocompletes(const std::string &partial,
                                           Node *nodes)
{
    vector<string> autocompletes;

    const Command &comm = Ast::get_last_command(nodes);
    if (comm.Args.empty())
    {
        return autocompletes;
    }

    std::string current_token = comm.Args.back();
    bool is_command = current_token == comm.Args[0] && partial.back() != ' ';

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
        autocompletes = m_complete_registry->Autocomplete(comm, partial);

        if (autocompletes.empty())
        {
            // collect files
            vector<std::string> files = CollectAutocompleteInDir(partial, comm);
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
                                                    const Command &comm)
{
    const string &token = comm.Args.back();
    std::string pwd = get_current_dir_name();
    std::string relative_path = token;

    if (token.ends_with('/'))
    {
        relative_path.pop_back();
    }

    if (token.contains('/'))
    {
        pwd = std::format("{}/{}", pwd,
                          token.substr(0, token.length() - (token.length() -
                                                            token.rfind('/'))));
        relative_path =
            token.substr(token.rfind('/') + 1,
                         token.length() - (token.length() - token.rfind('/')));
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

    return shell.m_executor.ReapBackgroundJobs();
}

void Shell::LineHandler(char *line)
{
    Shell &shell = Shell::Instance();

    if (line != nullptr)
    {
        shell.m_current_input = line;
        shell.m_history_registry->Add(line);
        free(line);
    }

    shell.m_line_ready = true;
    rl_callback_handler_remove();
}

void Shell::ExitShell(bool exit) { m_exit_shell = exit; }

int Shell::UpArrow(int count, int key)
{
    Shell &shell = Instance();

    shell.m_current_input = shell.m_history_registry->Up();
    rl_replace_line(shell.m_current_input.c_str(), 0);
    rl_point = rl_end;

    return 0;
}

int Shell::DownArrow(int count, int key)
{
    Shell &shell = Instance();

    shell.m_current_input = shell.m_history_registry->Down();
    rl_replace_line(shell.m_current_input.c_str(), 0);
    rl_point = rl_end;

    return 0;
}

void Shell::SetHistoryFile(const std::string &histfile)
{
    BuiltinCommand *comm = m_registry.GetCommandByName("history");
    if (comm == nullptr)
    {
        return;
    }

    auto *hist_comm = dynamic_cast<HistoryCommand *>(comm);

    if (hist_comm == nullptr)
    {
        return;
    }

    hist_comm->SetHistoryFile(histfile);
}
