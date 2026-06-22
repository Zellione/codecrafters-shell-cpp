#pragma once

#include "specialized_parser.h"

class DoubleQuotes : public SpecializedParser {
  public:
    virtual Token Parse(const std::string &command,
                        size_t start_pos) const override;
    virtual bool IsActivated(char c) const override;

    virtual NodeType GetNodeType() const override;
};
