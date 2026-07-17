#include "lexer.h"

#include <array>
#include <cstring>
#include <format>
#include <map>

using std::string;

Lexer::Lexer(const string &source)
    : m_source(source), m_pos(0),
      m_current_char(!source.empty() ? source[0] : 0), m_line(1), m_col(1),
      m_length(source.length())
{
}

void Lexer::Advance(int steps)
{
    for (int i = 0; i < steps; i++)
    {
        m_col++;
        if (m_current_char == '\n')
        {
            m_line++;
            m_col = 0;
        }

        m_pos++;
        m_current_char = m_pos < m_length ? m_source[m_pos] : 0;
    }
}

char Lexer::Peek(int steps)
{
    size_t peek_pos = m_pos + steps;

    if (peek_pos < m_length)
    {
        return m_source[peek_pos];
    }

    return 0;
}

void Lexer::SkipWhiteSpaces()
{
    static std::array<char, 3> whitespaces{' ', '\r', '\t'};
    while (m_current_char != 0 &&
           std::ranges::find(whitespaces, m_current_char) != whitespaces.end())
    {
        Advance();
    }
}

Ast::Token Lexer::NextToken()
{
    // 1. Skip trivial whitespaces, but not newlines
    SkipWhiteSpaces();

    // 2. Handle EOF
    if (m_current_char == 0)
    {
        return {Ast::TokenType::END_OF_FILE, "", m_line, m_col};
    }

    int start_line = m_line;
    int start_col = m_col;
    char tchar = m_current_char;

    // 3. Handle NEWLINE
    if (tchar == '\n')
    {
        Advance();
        return {Ast::TokenType::NEWLINE_TOKEN, "\n", start_line, start_col};
    }

    // 4. Handle COMMENT (hash outside a word)
    if (tchar == '#')
    {
        while (m_current_char != 0 && m_current_char != '\n')
        {
            Advance();
        }

        return NextToken(); // recusively get new tokens
    }

    // 5. Handle OPERATORS
    static std::array<char, 7> single_operators{'|', '&', ';', '(',
                                                ')', '>', '<'};
    if (tchar != 0 &&
        (std::ranges::find(single_operators, tchar) != single_operators.end() ||
         (m_current_char == '1' && Peek() == '>') ||
         (m_current_char == '2' && Peek() == '>')))
    {
        return ReadOperator();
    }

    // 6. Handle WORD
    return ReadWord();
}

Ast::Token Lexer::ReadOperator()
{
    int start_col = m_col;
    int start_line = m_line;
    char next_char = Peek();

    if (m_current_char == '|' && next_char == '|')
    {
        Advance(2);

        return {Ast::TokenType::DOUBLE_PIPE, "||", start_line, start_col};
    }

    if (m_current_char == '&' && next_char == '&')
    {
        Advance(2);

        return {Ast::TokenType::DOUBLE_AND, "&&", start_line, start_col};
    }

    if (m_current_char == '>' && next_char == '>')
    {
        Advance(2);

        return {Ast::TokenType::GREATER_GREATER, ">>", start_line, start_col};
    }

    if (m_current_char == '<' && next_char == '<')
    {
        Advance(2);

        return {Ast::TokenType::LESS_LESS, "<<", start_line, start_col};
    }

    if (m_current_char == '>' && next_char == '&')
    {
        Advance(2);

        return {Ast::TokenType::GREATER_AND, ">&", start_line, start_col};
    }

    if (m_current_char == '<' && next_char == '&')
    {
        Advance(2);

        return {Ast::TokenType::LESS_AND, "<&", start_line, start_col};
    }

    char next_next_char = Peek(2);
    if (next_next_char != 0)
    {
        if (m_current_char == '1' && next_char == '>' && next_next_char == '>')
        {
            Advance(3);
            return {Ast::TokenType::ONE_GREATER_GREATER, "1>>", start_line,
                    start_col};
        }

        if (m_current_char == '1' && next_char == '>')
        {
            Advance(2);
            return {Ast::TokenType::ONE_GREATER, "1>", start_line, start_col};
        }

        if (m_current_char == '2' && next_char == '>' && next_next_char == '>')
        {
            Advance(3);
            return {Ast::TokenType::TWO_GREATER_GREATER, "2>>", start_line,
                    start_col};
        }

        if (m_current_char == '2' && next_char == '>')
        {
            Advance(2);
            return {Ast::TokenType::TWO_GREATER, "2>", start_line, start_col};
        }
    }

    static std::map<char, Ast::TokenType> single_operators{
        {'|', Ast::TokenType::PIPE},      {'&', Ast::TokenType::AMPERSAND},
        {';', Ast::TokenType::SEMICOLON}, {'(', Ast::TokenType::LPAREN},
        {')', Ast::TokenType::RPAREN},    {'>', Ast::TokenType::GREATER},
        {'<', Ast::TokenType::LESS}};

    const auto &token_type = single_operators.find(m_current_char);
    if (token_type == single_operators.end())
    {
        throw std::format("Unknown operator: {}", m_current_char);
    }

    Advance();
    return {token_type->second, {m_current_char}, start_line, start_col};
}

Ast::Token Lexer::ReadWord()
{
    int start_col = m_col;
    int start_line = m_line;
    std::string value;

    char quote_char = 0;
    while (m_current_char != 0)
    {
        char tchar = m_current_char;

        if (tchar == '\\')
        {
            if (quote_char == '\'')
            {
                value.append("\\");
                Advance();

                continue;
            }

            Advance();
            if (m_current_char == 0)
            {
                value.append("\\");
                break;
            }

            char escaped = m_current_char;
            value.append({escaped});
            Advance();

            continue;
        }

        static std::array<char, 2> quotes{'"', '\''};
        if (tchar != 0 && std::ranges::find(quotes, tchar) != quotes.end())
        {
            if (quote_char == 0)
            {
                // Entering a quote
                quote_char = tchar;
                Advance();

                continue;
            }
            if (quote_char == tchar)
            {
                // Exiting the Quote
                quote_char = 0;
                Advance();

                continue;
            }

            value.append({tchar});
            Advance();

            continue;
        }

        static std::array<char, 11> stopping_conditions{
            ' ', '\t', '\r', '|', '&', ';', '(', ')', '<', '>'};
        if (quote_char == 0)
        {
            if (tchar != 0 && std::ranges::find(stopping_conditions, tchar) !=
                                  stopping_conditions.end())
            {
                break;
            }

            if (tchar == '#' && value.empty())
            {
                // Comment, skip line
                while (m_current_char != 0 && m_current_char != '\n')
                {
                    Advance();
                }
                break;
            }
        }

        // Normal char
        value.append({tchar});
        Advance();
    }

    if (quote_char != 0)
    {
        throw std::format("Unclosed {} quote at line {}, col {}", quote_char,
                          start_line, start_col);
    }

    return {Ast::TokenType::WORD, value, start_line, start_col};
}
