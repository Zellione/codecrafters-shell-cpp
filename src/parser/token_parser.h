#pragma once

#include "specialized_parser.h"
#include <vector>

struct Node {
    Node(std::string token, NodeType type) : token(token), type(type) {}

    std::string token;
    NodeType type;
};

class TokenParser {
  private:
    std::vector<SpecializedParser *> m_Parsers;

  public:
    TokenParser();

    std::vector<Node> GetCommandAndArgs(const std::string &commandline) const;
};
