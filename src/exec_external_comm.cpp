#include "exec_external_comm.h"
#include "helper.h"

#include <cstdio>
#include <filesystem>
#include <iomanip>
#include <sstream>

ExecExternalCommand::ExecExternalCommand(Output *output)
    : m_Parser(TokenParser()), m_Output(output) {}

bool ExecExternalCommand::Exec(const std::string &commandline) const {
    std::vector<Token> tokens = m_Parser.Parse(commandline);
    if (tokens.size() == 0)
        return false;

    std::filesystem::path exec_path = find_executable(tokens[0].token);
    if (exec_path == "")
        return false;

    std::stringstream exec_path_with_args;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type != TokenType::NORMAL)
            break;
        if (tokens[i].token.find(' ') != std::string::npos ||
            tokens[i].token.find('\'') != std::string::npos ||
            tokens[i].token.find('\\') != std::string::npos) {
            exec_path_with_args << std::quoted(tokens[i].token);
        } else {
            exec_path_with_args << tokens[i].token;
        }
        if (i != tokens.size() - 1)
            exec_path_with_args << " ";
    }

    FILE *pipe = popen(exec_path_with_args.str().c_str(), "r");
    if (!pipe)
        return false;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        m_Output->Put(tokens, buffer);
    }
    pclose(pipe);

    return true;
}
