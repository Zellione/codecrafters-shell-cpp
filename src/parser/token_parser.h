#pragma once

#include <string>
#include <vector>

enum class ParserState {
    NORMAL,
    INSIDE_SINGLE_QUOTES,
    INSIDE_DOUBLE_QUOTES,
    ON_BACKSLASH
};

class TokenParser {
  private:
    ParserState DetermineState(char c, ParserState current) const;
    std::string ParseInsideSingleQuotes(const std::string &commandline,
                                        size_t pos) const;
    std::string ParseInsideDoubleQuotes(const std::string &commandline,
                                        size_t pos) const;
    std::string ParsePreviousBackslash(const std::string &commandline,
                                       size_t pos) const;

  public:
    TokenParser();

    std::vector<std::string> Parse(const std::string &commandline) const;
};
