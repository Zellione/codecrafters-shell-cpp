#include "shell.h"
#include "commands/builtin/echo.h"
#include "commands/builtin/exit.h"
#include "commands/builtin/type.h"
#include "commands/error/err_not_found.h"
#include "output/console_output.h"
#include "output/redirect_std_err.h"
#include "output/redirect_std_out.h"
#include "readline/readline.h"

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

    std::string command = shell.m_registry.AutoComplete(rl_line_buffer);

    if (command.empty())
    {
        return 1;
    }

    command += " ";
    rl_replace_line(command.c_str(), 0);
    rl_point = rl_end;

    return 0;
}
