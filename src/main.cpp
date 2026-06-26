#include <iostream>
#include <string>

#include "builtin_command.h"
#include "builtin_registry.h"
#include "echo.h"
#include "exec_external_comm.h"
#include "exit.h"
#include "output/console_output.h"
#include "output/output.h"
#include "output/redirect_err_out.h"
#include "output/redirect_std_out.h"
#include "type.h"

int main() {
    Output output;
    output.AddType(new RedirectStdOut());
    output.AddType(new RedirectErrOut());
    output.AddType(new ConsoleOutput());

    BuiltinRegistry registry;
    EchoCommand echo_command(&output);
    ExitCommand exit_command(&output);
    TypeCommand type_command(&registry, &output);
    ExecExternalCommand exec_external_comm(&output);

    registry.RegisterCommand(&echo_command);
    registry.RegisterCommand(&exit_command);
    registry.RegisterCommand(&type_command);

    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        std::cout << "$ ";

        std::string user_input;
        std::getline(std::cin, user_input);

        const BuiltinCommand *command = registry.FindCommand(user_input);
        if (command != nullptr) {
            command->Execute(user_input);

            continue;
        }

        bool external_command = exec_external_comm.Exec(user_input);
        if (external_command) {
            continue;
        }

        std::cout << user_input << ": command not found" << '\n';
    }
}
