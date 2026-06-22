#include "token_parser.h"

TokenParser::TokenParser() : m_sqQuotes() {}

std::vector<std::string>
TokenParser::GetCommandAndArgs(const std::string &commandline) const {
    std::vector<std::string> comm_and_args;

    std::string buffer = "";
    size_t pos = 0;
    while (pos >= 0 && pos < commandline.length()) {
        if (m_sqQuotes.IsActivated(commandline[pos])) {
            Token token = m_sqQuotes.Parse(commandline, pos);
            pos = token.index_after;
            if (token.token != "")
                comm_and_args.emplace_back(token.token);
            continue;
        }

        if (commandline[pos] == ' ') {
            if (buffer.length() > 0)
                comm_and_args.emplace_back(buffer);
            buffer = "";
            while (commandline[pos] == ' ') {
                pos++;
            }
            continue;
        }

        buffer += commandline[pos];
        pos++;
    }
    if (buffer.length() > 0)
        comm_and_args.emplace_back(buffer);

    return comm_and_args;
}
