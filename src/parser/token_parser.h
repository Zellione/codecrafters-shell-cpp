#pragma once

#include <string>
#include <vector>

enum class ParserState {
    NORMAL,
    INSIDE_SINGLE_QUOTES,
    INSIDE_DOUBLE_QUOTES,
    ON_BACKSLASH
};

struct Token {
    Token() : start_pos(0), end_pos(0), buffer("") {}
    Token(size_t start_pos, size_t end_pos, std::string buffer)
        : start_pos(start_pos), end_pos(end_pos), buffer(buffer) {}
    size_t start_pos;
    size_t end_pos;
    std::string buffer;
};

class TokenParser {
  private:
    ParserState DetermineState(char c, ParserState current) const;
    Token ParseInsideSingleQuotes(const std::string &commandline,
                                  size_t pos) const;
    Token ParseInsideDoubleQuotes(const std::string &commandline,
                                  size_t pos) const;
    Token ParsePreviousBackslash(const std::string &commandline,
                                 size_t pos) const;

  public:
    TokenParser();

    std::vector<std::string> Parse(const std::string &commandline) const;
};
