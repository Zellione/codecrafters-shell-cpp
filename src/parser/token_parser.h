#pragma once

#include <string>
#include <vector>

enum class ParserState {
    NORMAL,
    INSIDE_SINGLE_QUOTES,
    INSIDE_DOUBLE_QUOTES,
    ON_BACKSLASH,
    REDIRECT_STDOUT,
    REDIRECT_STDERR
};

enum class TokenType { NORMAL, REDIRECT_STDOUT, REDIRECT_STDERR };

struct Token {
    Token(std::string token, TokenType type) : token(token), type(type) {}
    std::string token;
    TokenType type;
};

struct InternalToken {
    InternalToken() : start_pos(0), end_pos(0), buffer("") {}
    InternalToken(size_t start_pos, size_t end_pos, std::string buffer)
        : start_pos(start_pos), end_pos(end_pos), buffer(buffer) {}
    size_t start_pos;
    size_t end_pos;
    std::string buffer;
};

class TokenParser {
  private:
    ParserState DetermineState(const std::string &commandline, size_t pos,
                               ParserState current) const;
    TokenType DetermineTokenType(ParserState state) const;

    InternalToken ParseInsideSingleQuotes(const std::string &commandline,
                                          size_t start_pos) const;
    InternalToken ParseInsideDoubleQuotes(const std::string &commandline,
                                          size_t start_pos) const;
    InternalToken ParsePreviousBackslash(const std::string &commandline,
                                         size_t start_pos) const;

    InternalToken ParseStdOutRedirect(const std::string &commandline,
                                      size_t start_pos) const;

  public:
    TokenParser();

    std::vector<Token> Parse(const std::string &commandline) const;
};
