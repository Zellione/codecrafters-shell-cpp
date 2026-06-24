#include "exec_external_comm.h"
#include "helper.h"

#include <cstdio>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

ExecExternalCommand::ExecExternalCommand() : m_Parser(TokenParser()) {}

bool ExecExternalCommand::Exec(std::string commandline) const {
    std::vector<std::string> comms_and_args = m_Parser.Parse(commandline);
    if (comms_and_args.size() == 0)
        return false;

    std::filesystem::path exec_path = find_executable(comms_and_args[0]);
    if (exec_path == "")
        return false;

    std::stringstream exec_path_with_args;
    for (int i = 0; i < comms_and_args.size(); i++) {
        if (comms_and_args[i].find(' ') != std::string::npos ||
            comms_and_args[i].find('\'') != std::string::npos ||
            comms_and_args[i].find('\\') != std::string::npos) {
            exec_path_with_args << std::quoted(comms_and_args[i]);
        } else {
            exec_path_with_args << comms_and_args[i];
        }
        if (i != comms_and_args.size() - 1)
            exec_path_with_args << " ";
    }

    FILE *pipe = popen(exec_path_with_args.str().c_str(), "r");
    if (!pipe)
        return false;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer;
    }
    pclose(pipe);

    return true;
}
