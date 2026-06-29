#include "shell.h"
#include "commands/builtin/complete.h"
#include "commands/builtin/echo.h"
#include "commands/builtin/exit.h"
#include "commands/builtin/type.h"
#include "commands/error/err_not_found.h"
#include "helper/filesystem.h"
#include "output/console_output.h"
#include "output/redirect_stderr.h"
#include "output/redirect_stdout.h"
#include "readline/readline.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

Shell::Shell() : m_external_comm(&m_output)
{
    m_output.AddType(new RedirectStdOut());
    m_output.AddType(new RedirectStdErr());
    m_output.AddType(new ConsoleOutput());

    m_registry.RegisterCommand(new EchoCommand(&m_output));
    m_registry.RegisterCommand(new ExitCommand(&m_output));
    m_registry.RegisterCommand(new TypeCommand(&m_registry, &m_output));
    m_registry.RegisterCommand(new CompleteCommand(&m_output));
}

void Shell::run()
{
    const char *user_input;
    while ((user_input = readline("$ ")) != nullptr)
    {
        std::vector<Token> tokens = TokenParser::Parse(user_input);

        const BuiltinCommand *command = m_registry.FindCommand(tokens);
        if (command != nullptr)
        {
            command->Execute(tokens);

            continue;
        }

        bool ext_command = m_external_comm.Exec(tokens);
        if (ext_command)
        {
            continue;
        }

        ErrorNotFound::Raise(tokens);

        delete user_input;
    }
}

int Shell::TabAutoComplete(int count, int key)
{
    Shell &shell = Shell::Instance();

    if (rl_line_buffer == shell.m_lastprompt && shell.m_autocomplete.size() > 1)
    {
        return Shell::TabAutoCompleteMulti(count, key);
    }
    std::vector<Token> tokens = TokenParser::Parse(rl_line_buffer);

    shell.m_autocomplete = shell.CollectAutocompletes(rl_line_buffer, tokens);
    shell.m_lastprompt = rl_line_buffer;

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

    std::vector<Token> tokens{};
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

std::vector<std::string>
Shell::CollectAutocompletes(const std::string &partial,
                            const std::vector<Token> &tokens)
{
    std::vector<std::string> autocompletes;

    if (tokens.empty())
    {
        return autocompletes;
    }

    const Token &token = tokens.back();
    std::string current_token = token.token;
    bool is_command = token.type == TokenType::COMMAND && partial.back() != ' ';

    if (current_token.starts_with(m_lastprompt) && partial.back() != ' ')
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
        std::vector<std::string> ext_commands =
            CollectAutocompleteInPath(current_token);
        autocompletes.insert(autocompletes.end(), ext_commands.begin(),
                             ext_commands.end());
    }
    else
    {
        // collect files
        autocompletes = CollectAutocompleteInDir(partial, tokens);
    }

    // Sort commands
    std::ranges::sort(autocompletes);
    autocompletes.erase(
        std::unique(autocompletes.begin(), autocompletes.end()), // NOLINT
        autocompletes.end());

    return autocompletes;
}

std::vector<std::string>
Shell::CollectAutocompleteInPath(const std::string &partial)
{
    return ExternalCommand::SearchExecutable(partial);
}

std::vector<std::string>
Shell::CollectAutocompleteBuiltin(const std::string &partial) const
{
    return m_registry.AutoComplete(partial);
}

std::vector<std::string>
Shell::CollectAutocompleteInDir(const std::string &partial,
                                const std::vector<Token> &tokens)
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

    std::vector<std::string> files = get_files_from_dir(pwd);
    std::vector<std::string> dirs = get_subdirs_from_dir(pwd);
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
