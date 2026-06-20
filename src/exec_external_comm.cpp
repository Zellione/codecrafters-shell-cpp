#include "exec_external_comm.h"
#include "helper.h"

#include <cstdio>
#include <filesystem>
#include <iostream>

bool ExecExternalCommand::Exec(std::string commandline) const {
    std::vector<std::string> comms_and_args = get_command_and_args(commandline);
    if (comms_and_args.size() == 0)
        return false;

    std::filesystem::path exec_path = find_executable(comms_and_args[0]);
    if (exec_path == "")
        return false;

    std::string exec_path_with_args = "";
    for (int i = 0; i < comms_and_args.size(); i++) {
        exec_path_with_args += comms_and_args[i];
        if (i != comms_and_args.size() - 1)
            exec_path_with_args += " ";
    }

    FILE *pipe = popen(exec_path_with_args.c_str(), "r");
    if (!pipe)
        return false;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer;
    }
    pclose(pipe);

    return true;
}
