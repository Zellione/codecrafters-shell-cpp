#pragma once

#include "ast.h"
#include "lexer.h"

class Parser
{
  private:
    Lexer &m_lexer;
    Ast::Token m_current_token;

    Ast::Node *ParseSequence();
    Ast::Node *ParsePipeline();
    Ast::Command ParseCommand();

    void Eat(Ast::TokenType token_type);

  public:
    Parser(Lexer &lexer);

    Ast::Node *Parse();
};
