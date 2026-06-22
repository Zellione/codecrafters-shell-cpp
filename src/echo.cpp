#include "echo.h"

#include <iostream>

EchoCommand::EchoCommand()
    : BuiltinCommand("echo", "echo is a shell builtin") {}

void EchoCommand::Process(std::vector<Node> arguments) const {
    for (int i = 2; i < arguments.size(); i++) {

        std::cout << arguments[i].token;
    }
    std::cout << std::endl;
}
