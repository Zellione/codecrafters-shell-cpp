#include "echo.h"

#include <iostream>

EchoCommand::EchoCommand()
    : BuiltinCommand("echo", "echo is a shell builtin") {}

void EchoCommand::Process(std::vector<std::string> arguments) const {
  for (int i = 0; i < arguments.size(); i++) {
    std::cout << arguments[i];
    if (i != arguments.size() + 1)
      std::cout << " ";
  }
  std::cout << std::endl;
}
