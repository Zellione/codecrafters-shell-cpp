#pragma once

#include <array>
#include <cstdint>
#include <format>
#include <iostream>
#include <string>

#define STRINGIFY_TOKEN(x) #x

namespace Ast
{

enum class TokenType : uint8_t
{
    // Special
    END_OF_FILE,
    NEWLINE_TOKEN,

    // Literals
    WORD,

    // Operators (single char)
    PIPE,      // |
    AMPERSAND, // &
    SEMICOLON, // ;
    LPAREN,    // (
    RPAREN,    // )
    LBRACE,    // { (used for grouping, must be recognized)
    RBRACE,    // }
    LBRACKET,  // [ (test command, often a builtin, but lexer sees it)
    RBRACKET,  // ]
    GREATER,   // >
    LESS,      // <
    BANG,      // ! (history expansion / negation)

    // Operators (double char)
    DOUBLE_PIPE,     // ||
    DOUBLE_AND,      // &&
    GREATER_GREATER, // >>
    LESS_LESS,       // <<  (heredoc)
    GREATER_AND,     // >& (duplicate stdout)
    LESS_AND,        // <& (duplicate stdin)
    PIPE_AND,        // |& (pipe both stdout and stderr)
    GREATER_BAR,     // >| (force redirection, noclobber override)
    LESS_LESS_DASH,  // <<- (heredoc with leading tabs stripped)
    LESS_GREATER,    // <> (open file for reading and writing)

    // Redirection number
    ONE_GREATER,         // 1>
    ONE_GREATER_GREATER, // 1>>
    TWO_GREATER,         // 2>
    TWO_GREATER_GREATER  // 2>>
};

const std::array<std::string, 29> TokenTypeNames{
    STRINGIFY_TOKEN(TokenType::END_OF_FILE),
    STRINGIFY_TOKEN(TokenType::NEWLINE_TOKEN),
    STRINGIFY_TOKEN(TokenType::WORD),
    STRINGIFY_TOKEN(TokenType::PIPE),
    STRINGIFY_TOKEN(TokenType::AMPERSAND),
    STRINGIFY_TOKEN(TokenType::SEMICOLON),
    STRINGIFY_TOKEN(TokenType::LPAREN),
    STRINGIFY_TOKEN(TokenType::RPAREN),
    STRINGIFY_TOKEN(TokenType::LBRACE),
    STRINGIFY_TOKEN(TokenType::RBRACE),
    STRINGIFY_TOKEN(TokenType::LBRACKET),
    STRINGIFY_TOKEN(TokenType::RBRACKET),
    STRINGIFY_TOKEN(TokenType::GREATER),
    STRINGIFY_TOKEN(TokenType::LESS),
    STRINGIFY_TOKEN(TokenType::BANG),
    STRINGIFY_TOKEN(TokenType::DOUBLE_PIPE),
    STRINGIFY_TOKEN(TokenType::DOUBLE_AND),
    STRINGIFY_TOKEN(TokenType::GREATER_GREATER),
    STRINGIFY_TOKEN(TokenType::LESS_LESS),
    STRINGIFY_TOKEN(TokenType::GREATER_AND),
    STRINGIFY_TOKEN(TokenType::LESS_AND),
    STRINGIFY_TOKEN(TokenType::PIPE_AND),
    STRINGIFY_TOKEN(TokenType::GREATER_BAR),
    STRINGIFY_TOKEN(TokenType::LESS_LESS_DASH),
    STRINGIFY_TOKEN(TokenType::LESS_GREATER),
    STRINGIFY_TOKEN(TokenType::ONE_GREATER),
    STRINGIFY_TOKEN(TokenType::ONE_GREATER_GREATER),
    STRINGIFY_TOKEN(TokenType::TWO_GREATER),
    STRINGIFY_TOKEN(TokenType::TWO_GREATER_GREATER)};

struct Token
{
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType type, std::string value, int line, int column)
        : type(type), value(std::move(value)), line(line), column(column)
    {
    }
    Token() = default;

    void Print() const
    {
        std::cout << std::format("Token({}, \"{}\", L{}:C{})\n",
                                 TokenTypeNames[static_cast<int>(type)], value,
                                 line, column);
    }
};

}; // namespace Ast
