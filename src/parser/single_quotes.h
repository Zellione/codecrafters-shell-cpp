#pragma once

#include <string>

struct Token {
    Token(std::string token, size_t index_after)
        : token(token), index_after(index_after) {}

    std::string token;
    size_t index_after;
};

class SingleQuotes {
  private:
    bool IsEndOfCommand(const std::string &command, size_t pos) const;
    bool HasAdjacentSingleQuotes(const std::string &command,
                                 size_t start_pos) const;

  public:
    Token Parse(const std::string &command, size_t start_pos) const;
    bool IsActivated(char c) const;
    bool HasSingleQuotes(const std::string &command) const;
};
