#include "shell.h"
#include "commands/builtin/echo.h"
#include "commands/builtin/exit.h"
#include "commands/builtin/type.h"
#include "commands/error/err_not_found.h"
#include "output/console_output.h"
#include "output/redirect_stderr.h"
#include "output/redirect_stdout.h"
#include "readline/readline.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Shell::Shell() : m_external_comm(&m_output)
{
    m_output.AddType(new RedirectStdOut());
    m_output.AddType(new RedirectStdErr());
    m_output.AddType(new ConsoleOutput());

    m_registry.RegisterCommand(new EchoCommand(&m_output));
    m_registry.RegisterCommand(new ExitCommand(&m_output));
    m_registry.RegisterCommand(new TypeCommand(&m_registry, &m_output));
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

    shell.m_autocomplete = shell.CollectAutocompletes(rl_line_buffer);
    shell.m_lastprompt = rl_line_buffer;

    if (shell.m_autocomplete.empty())
    {
        std::cout << '\x07';
        return 1;
    }

    if (shell.m_autocomplete.size() == 1)
    {
        std::string command = shell.m_autocomplete[0] + " ";
        rl_replace_line(command.c_str(), 0);
        rl_point = rl_end;

        return 0;
    }

    std::cout << '\x07';

    rl_replace_line(shell.LongestCommonPrefix(rl_line_buffer).c_str(), 0);
    rl_point = rl_end;

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
Shell::CollectAutocompletes(const std::string &partial) const
{
    std::vector<std::string> commands = m_registry.AutoComplete(partial);
    std::vector<std::string> ext_commands =
        ExternalCommand::SearchExecutable(partial);
    commands.insert(commands.end(), ext_commands.begin(), ext_commands.end());

    std::ranges::sort(commands);
    commands.erase(std::unique(commands.begin(), commands.end()),
                   commands.end());

    return commands;
}

std::string Shell::LongestCommonPrefix(const std::string &partial) const
{
    std::string autocomplete = partial;
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
