#pragma once

#include "single_quotes.h"
#include <vector>

class TokenParser {
  private:
    SingleQuotes m_sqQuotes;

  public:
    TokenParser();

    std::vector<std::string>
    GetCommandAndArgs(const std::string &commandline) const;
};
