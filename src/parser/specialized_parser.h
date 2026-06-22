#pragma once

#include <string>

enum class NodeType { NONE, SPACE, COMMAND, SINGLE_QUOTES, DOUBLE_QUOTES };

struct Token {
    Token(std::string token, size_t index_after)
        : token(token), index_after(index_after) {}

    std::string token;
    size_t index_after;
};

class SpecializedParser {
  protected:
    bool IsEndOfCommand(const std::string &command, size_t pos) const;

  public:
    SpecializedParser();
    ~SpecializedParser();

    virtual Token Parse(const std::string &command, size_t start_pos) const = 0;
    virtual bool IsActivated(char c) const = 0;
    virtual NodeType GetNodeType() const = 0;
};
