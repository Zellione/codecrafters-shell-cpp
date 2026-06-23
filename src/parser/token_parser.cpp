#include "token_parser.h"

TokenParser::TokenParser() {}

std::vector<std::string>
TokenParser::Parse(const std::string &commandline) const {
    std::vector<std::string> comm_and_args;

    ParserState state = ParserState::NORMAL;
    std::string buffer = "";
    size_t pos = 0;
    while (pos < commandline.length()) {
        state = DetermineState(commandline[pos], state);
        switch (state) {
        case ParserState::INSIDE_SINGLE_QUOTES:
            buffer += ParseInsideSingleQuotes(commandline, pos);
            pos += buffer.length() + 1;
            break;
        case ParserState::INSIDE_DOUBLE_QUOTES:
            buffer += ParseInsideDoubleQuotes(commandline, pos);
            pos += buffer.length() + 1;

            break;
        case ParserState::ON_BACKSLASH:
            buffer += ParsePreviousBackslash(commandline, pos);
            pos += 1;
            break;
        case ParserState::NORMAL:
        default:
            if (commandline[pos] != ' ')
                buffer += commandline[pos];
            if (commandline[pos] == ' ')
                while (commandline[pos + 1] == ' ')
                    pos++;

            break;
        }

        if (pos + 1 >= commandline.length() ||
            (commandline[pos] == ' ' && state != ParserState::ON_BACKSLASH)) {
            comm_and_args.emplace_back(buffer);
            buffer = "";
        }

        pos++;
    }

    return comm_and_args;
}

ParserState TokenParser::DetermineState(char c, ParserState current) const {
    if (c == '\'')
        return ParserState::INSIDE_SINGLE_QUOTES;
    if (c == '"')
        return ParserState::INSIDE_DOUBLE_QUOTES;
    if (c == '\\')
        return ParserState::ON_BACKSLASH;

    return ParserState::NORMAL;
}

std::string TokenParser::ParseInsideSingleQuotes(const std::string &commandline,
                                                 size_t pos) const {
    std::string buffer = "";
    pos++;
    while (pos < commandline.length() && commandline[pos] != '\'') {
        buffer += commandline[pos];
        pos++;
    }

    return buffer;
}
std::string TokenParser::ParseInsideDoubleQuotes(const std::string &commandline,
                                                 size_t pos) const {
    std::string buffer = "";
    pos++;
    while (pos < commandline.length() && commandline[pos] != '"') {
        buffer += commandline[pos];
        pos++;
    }

    return buffer;
}
std::string TokenParser::ParsePreviousBackslash(const std::string &commandline,
                                                size_t pos) const {
    std::string buffer = "";

    buffer += commandline[pos + 1];

    return buffer;
}
