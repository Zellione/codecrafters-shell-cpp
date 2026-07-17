#pragma once

#include <string>

#include "token.h"

class Lexer
{
  private:
    const std::string &m_source;
    size_t m_pos;
    char m_current_char;
    int m_line;
    int m_col;
    size_t m_length;

    char Peek(int steps = 1);
    void SkipWhiteSpaces();
    void Advance(int steps = 1);

    Ast::Token ReadWord();
    Ast::Token ReadOperator();

  public:
    Lexer(const std::string &source);
    Lexer(const Lexer &other) = delete;
    ~Lexer() = default;

    Ast::Token NextToken();
};
