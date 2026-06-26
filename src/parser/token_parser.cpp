#include "token_parser.h"

#include "../helper.h"

std::vector<Token> TokenParser::Parse(const std::string &commandline) {
    std::vector<Token> tokens = std::vector<Token>();

    ParserState state = ParserState::NORMAL;
    std::string buffer;
    size_t pos = 0;
    while (pos < commandline.length()) {
        state = DetermineState(commandline, pos, state);
        InternalToken tmp;
        switch (state) {
        case ParserState::INSIDE_SINGLE_QUOTES:
            tmp = ParseInsideSingleQuotes(commandline, pos);
            buffer += tmp.buffer;
            pos = tmp.end_pos;
            break;
        case ParserState::INSIDE_DOUBLE_QUOTES:
            tmp = ParseInsideDoubleQuotes(commandline, pos);
            buffer += tmp.buffer;
            pos = tmp.end_pos;
            break;
        case ParserState::ON_BACKSLASH:
            buffer += ParsePreviousBackslash(commandline, pos).buffer;
            pos += 1;
            break;
        case ParserState::REDIRECT_STDOUT:
        case ParserState::REDIRECT_STDERR:
            tmp = ParseRedirect(commandline, pos);
            buffer += tmp.buffer;
            pos = tmp.end_pos;
            break;
        case ParserState::NORMAL:
        default:
            if (commandline[pos] != ' ') {
                buffer += commandline[pos];
            }
            if (commandline[pos] == ' ') {
                while (pos + 1 < commandline.length() &&
                       commandline[pos + 1] == ' ') {
                    pos++;
                }
            }

            break;
        }

        if (pos + 1 >= commandline.length() ||
            (commandline[pos] == ' ' && state != ParserState::ON_BACKSLASH)) {
            tokens.emplace_back(buffer, DetermineTokenType(state));
            buffer = "";
        }

        pos++;
    }

    return tokens;
}

TokenType TokenParser::DetermineTokenType(ParserState state) {
    switch (state) {
    case ParserState::REDIRECT_STDOUT:
        return TokenType::REDIRECT_STDOUT;
    case ParserState::REDIRECT_STDERR:
        return TokenType::REDIRECT_STDERR;
    default:
        break;
    }

    return TokenType::NORMAL;
}

ParserState TokenParser::DetermineState(const std::string &commandline,
                                        size_t pos, ParserState current) {
    if (commandline[pos] == '\'') {
        return ParserState::INSIDE_SINGLE_QUOTES;
    }
    if (commandline[pos] == '"') {
        return ParserState::INSIDE_DOUBLE_QUOTES;
    }
    if (commandline[pos] == '\\') {
        return ParserState::ON_BACKSLASH;
    }
    if (commandline[pos] == '>' ||
        (commandline[pos] == '1' && commandline[pos + 1] == '>')) {
        return ParserState::REDIRECT_STDOUT;
    }
    if (commandline[pos] == '2' && commandline[pos + 1] == '>') {
        return ParserState::REDIRECT_STDERR;
    }

    return ParserState::NORMAL;
}

InternalToken
TokenParser::ParseInsideSingleQuotes(const std::string &commandline,
                                     size_t start_pos) {
    std::string buffer;
    size_t pos = start_pos;
    pos++;
    while (pos < commandline.length() && commandline[pos] != '\'') {
        buffer += commandline[pos];
        pos++;
    }

    return {start_pos, pos, buffer};
}
InternalToken
TokenParser::ParseInsideDoubleQuotes(const std::string &commandline,
                                     size_t start_pos) {
    std::string buffer;
    size_t pos = start_pos;
    pos++;
    while (pos < commandline.length() && commandline[pos] != '"') {
        if (commandline[pos] == '\\') {
            pos++;
        }
        buffer += commandline[pos];
        pos++;
    }

    return {start_pos, pos, buffer};
}
InternalToken
TokenParser::ParsePreviousBackslash(const std::string &commandline,
                                    size_t start_pos) {
    std::string buffer;
    size_t pos = start_pos + 1;
    if (pos < commandline.length()) {
        buffer += commandline[pos];
    }

    return {start_pos, pos, buffer};
}

InternalToken TokenParser::ParseRedirect(const std::string &commandline,
                                         size_t start_pos) {
    std::string buffer;
    size_t pos = start_pos + 1;

    if (commandline[pos] == '>') {
        pos++;
    }

    while (pos < commandline.length() && commandline[pos] != '>') {
        buffer += commandline[pos];
        pos++;
    }
    ltrim(buffer);

    return {start_pos, pos, buffer};
}
