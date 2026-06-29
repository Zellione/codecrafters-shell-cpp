#include "token_parser.h"

#include "../helper.h"

std::vector<Token> TokenParser::Parse(const std::string &commandline)
{
    std::vector<Token> tokens = std::vector<Token>();

    ParserState state = ParserState::NORMAL;
    std::string buffer;
    size_t pos = 0;
    size_t last_start_pos = 0;
    while (pos < commandline.length())
    {
        state = DetermineState(commandline, pos, state);
        InternalToken tmp;
        switch (state)
        {
        case ParserState::INSIDE_SINGLE_QUOTES:
            tmp = ParseInsideSingleQuotes(commandline, pos);
            buffer += tmp.token;
            pos = tmp.end_pos;
            break;
        case ParserState::INSIDE_DOUBLE_QUOTES:
            tmp = ParseInsideDoubleQuotes(commandline, pos);
            buffer += tmp.token;
            pos = tmp.end_pos;
            break;
        case ParserState::ON_BACKSLASH:
            buffer += ParsePreviousBackslash(commandline, pos).token;
            pos += 1;
            break;
        case ParserState::REDIRECT_STDOUT:
        case ParserState::REDIRECT_STDERR:
            tmp = ParseRedirect(commandline, pos);
            buffer += tmp.token;
            pos = tmp.end_pos;
            break;
        case ParserState::REDIRECT_STDOUT_APPEND:
        case ParserState::REDIRECT_STDERR_APPEND:
            tmp = ParseRedirect(commandline, pos + 1);
            buffer += tmp.token;
            pos = tmp.end_pos;
            break;
        case ParserState::FLAG:
            tmp = ParseFlag(commandline, pos);
            buffer = tmp.token;
            pos = tmp.end_pos;
            break;
        case ParserState::NORMAL:
        default:
            if (commandline[pos] != ' ')
            {
                buffer += commandline[pos];
            }
            if (commandline[pos] == ' ')
            {
                while (pos + 1 < commandline.length() &&
                       commandline[pos + 1] == ' ')
                {
                    pos++;
                }
            }

            break;
        }

        if (pos + 1 >= commandline.length() ||
            (commandline[pos] == ' ' && state != ParserState::ON_BACKSLASH))
        {
            TokenType tokenType =
                DetermineTokenType(state, commandline, buffer, last_start_pos);
            tokens.emplace_back(buffer, tokenType);
            last_start_pos = pos + 1;
            buffer = "";
        }

        pos++;
    }

    return tokens;
}

TokenType TokenParser::DetermineTokenType(ParserState state,
                                          const std::string &commandline,
                                          const std::string &buffer,
                                          size_t start_pos)
{
    switch (state)
    {
    case ParserState::REDIRECT_STDOUT:
        return TokenType::REDIRECT_STDOUT;
    case ParserState::REDIRECT_STDERR:
        return TokenType::REDIRECT_STDDERR;
    case ParserState::REDIRECT_STDOUT_APPEND:
        return TokenType::REDIRECT_STDOUT_APPEND;
    case ParserState::REDIRECT_STDERR_APPEND:
        return TokenType::REDIRECT_STDERR_APPEND;
    case ParserState::FLAG:
        return TokenType::FLAG;
    default:
        break;
    }

    if (start_pos == 0)
    {
        return TokenType::COMMAND;
    }

    if (buffer.contains('/'))
    {
        return TokenType::FILE_PATH;
    }

    if (buffer.ends_with('/'))
    {
        return TokenType::DIR_PATH;
    }

    return TokenType::TEXT;
}

ParserState TokenParser::DetermineState(const std::string &commandline,
                                        size_t pos, ParserState current)
{
    if (commandline[pos] == '\'')
    {
        return ParserState::INSIDE_SINGLE_QUOTES;
    }
    if (commandline[pos] == '"')
    {
        return ParserState::INSIDE_DOUBLE_QUOTES;
    }
    if (commandline[pos] == '\\')
    {
        return ParserState::ON_BACKSLASH;
    }
    if (commandline[pos] == '-')
    {
        return ParserState::FLAG;
    }
    if ((commandline[pos] == '>' && commandline[pos + 1] == '>') ||
        (commandline[pos] == '1' && commandline[pos + 1] == '>' &&
         commandline[pos + 2] == '>'))
    {
        return ParserState::REDIRECT_STDOUT_APPEND;
    }
    if (commandline[pos] == '2' && commandline[pos + 1] == '>' &&
        commandline[pos + 2] == '>')
    {
        return ParserState::REDIRECT_STDERR_APPEND;
    }
    if (commandline[pos] == '>' ||
        (commandline[pos] == '1' && commandline[pos + 1] == '>'))
    {
        return ParserState::REDIRECT_STDOUT;
    }
    if (commandline[pos] == '2' && commandline[pos + 1] == '>')
    {
        return ParserState::REDIRECT_STDERR;
    }

    return ParserState::NORMAL;
}

InternalToken
TokenParser::ParseInsideSingleQuotes(const std::string &commandline,
                                     size_t start_pos)
{
    std::string buffer;
    size_t pos = start_pos + 1;
    while (pos < commandline.length() && commandline[pos] != '\'')
    {
        buffer += commandline[pos];
        pos++;
    }

    return {start_pos, pos, buffer};
}
InternalToken
TokenParser::ParseInsideDoubleQuotes(const std::string &commandline,
                                     size_t start_pos)
{
    std::string buffer;
    size_t pos = start_pos + 1;
    while (pos < commandline.length() && commandline[pos] != '"')
    {
        if (commandline[pos] == '\\')
        {
            pos++;
        }
        buffer += commandline[pos];
        pos++;
    }

    return {start_pos, pos, buffer};
}
InternalToken
TokenParser::ParsePreviousBackslash(const std::string &commandline,
                                    size_t start_pos)
{
    std::string buffer;
    size_t pos = start_pos + 1;
    if (pos < commandline.length())
    {
        buffer += commandline[pos];
    }

    return {start_pos, pos, buffer};
}

InternalToken TokenParser::ParseRedirect(const std::string &commandline,
                                         size_t start_pos)
{
    std::string buffer;
    size_t pos = start_pos + 1;
    if (commandline[pos] == '>')
    {
        pos++;
    }

    while (pos < commandline.length() && commandline[pos] != '>')
    {
        if (commandline[pos] == '"')
        {
            pos++;
            continue;
        }
        buffer += commandline[pos];
        pos++;
    }
    ltrim(buffer);

    return {start_pos, pos, buffer};
}

InternalToken TokenParser::ParseFlag(const std::string &commandline,
                                     size_t start_pos)
{
    size_t pos = start_pos;
    size_t param_key_end = commandline.find(' ', pos);
    if (param_key_end == std::string::npos)
    {
        param_key_end = commandline.length() - 1;
    }
    std::string key = commandline.substr(pos, param_key_end - pos);

    return {start_pos, param_key_end, key};
}
