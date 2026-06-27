#pragma once

#include <vector>

#include "../../parser/token_parser.h"

class ErrorNotFound
{
  public:
    static void Raise(const std::vector<Token> &tokens);
};
