#include "single_quotes.h"

#include <string>

bool SingleQuotes::IsEndOfCommand(const std::string &command,
                                  size_t pos) const {
    return pos + 1 == command.length();
}

bool SingleQuotes::HasAdjacentSingleQuotes(const std::string &command,
                                           size_t start_pos) const {
    size_t pos = start_pos + 1;
    if (command[pos] != '\'')
        return false;

    return command.find('\'', pos) != std::string::npos;
}

Token SingleQuotes::Parse(const std::string &command, size_t start_pos) const {

    std::string token = "";
    size_t pos = start_pos + 1;
    while (pos < command.length()) {
        if (command[pos] == '\'') {
            if (IsEndOfCommand(command, pos))
                return Token(token, -1);
            if (HasAdjacentSingleQuotes(command, pos))
                pos += 2;
            else
                return Token(token, pos + 1);
        }

        size_t next = command.find('\'', pos);

        if (next == std::string::npos)
            return Token(token, pos + 1);

        size_t token_length = next - pos;
        token += command.substr(pos, token_length);

        pos = next;
    }

    return Token(token, -1);
}

bool SingleQuotes::IsActivated(char c) const { return c == '\''; }

bool SingleQuotes::HasSingleQuotes(const std::string &command) const {
    return command.find('\'') != std::string::npos;
}
