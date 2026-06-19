#include <iostream>
#include <string>

#include "builtin_command.h"
#include "builtin_registry.h"
#include "echo.h"
#include "exit.h"
#include "type.h"

int main() {
  BuiltinRegistry registry;
  EchoCommand echo_command;
  ExitCommand exit_command;
  TypeCommand type_command(&registry);

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

    if (command) {
      command->Execute(user_input);

      continue;
    }

    std::cout << user_input << ": command not found" << std::endl;
  }
}
