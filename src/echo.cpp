#include "echo.h"
#include "parser/token_parser.h"
#include <sstream>

EchoCommand::EchoCommand(Output *output)
    : BuiltinCommand("echo", "echo is a shell builtin", output) {}

void EchoCommand::Process(const std::vector<Token> &tokens) const {

    std::stringstream ss;
    for (size_t i = 1; i < tokens.size(); i++) {
        if (tokens[i].type != TokenType::NORMAL) {
            break;
        }

        ss << tokens[i].token;
        if (i < tokens.size() - 1) {
            ss << " ";
        }
    }
    ss << '\n';

    m_output->Put(tokens, ss.str(), OutputTarget::STDOUT);
    m_output->Put(tokens, "", OutputTarget::ERROUT);
}
