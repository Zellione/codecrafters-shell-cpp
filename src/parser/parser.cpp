#include "parser.h"
#include "token.h"

#include <algorithm>
#include <array>
#include <format>

using Ast::Command;
using Ast::Node;
using Ast::Pipeline;
using Ast::Redirect;
using Ast::Sequence;

Parser::Parser(Lexer &lexer) : m_lexer(lexer)
{
    m_current_token = m_lexer.NextToken();
}

Node *Parser::ParseSequence()
{
    Node *left = ParsePipeline();

    static std::array<Ast::TokenType, 3> applicableToken{
        Ast::TokenType::SEMICOLON, Ast::TokenType::DOUBLE_AND,
        Ast::TokenType::DOUBLE_PIPE};
    while (std::ranges::find(applicableToken, m_current_token.type) !=
           applicableToken.end())
    {
        Ast::TokenType op = m_current_token.type;
        Eat(op);
        Node *Right = ParsePipeline();
        left = new Node(new Ast::Node::NodeClass(new Sequence(left, op, Right)),
                        Ast::NodeType::SEQUENCE);
    }

    return left;
}

Node *Parser::ParsePipeline()
{
    Command cmd = ParseCommand();
    std::vector<Command> commands;
    commands.push_back(cmd);

    while (m_current_token.type == Ast::TokenType::PIPE)
    {
        Eat(Ast::TokenType::PIPE);
        commands.push_back(ParseCommand());
    }

    return new Node{new Ast::Node::NodeClass(new Pipeline(commands)),
                    Ast::NodeType::PIPELINE};
}

Ast::Command Parser::ParseCommand()
{
    std::vector<std::string> args;
    std::vector<Redirect> redirects;
    bool background = false;

    // A command has to start with a word
    if (m_current_token.type != Ast::TokenType::WORD)
    {
        throw "Unexpected token, expected command name";
    }

    args.push_back(m_current_token.value);
    Eat(Ast::TokenType::WORD);

    // Collect arguments, redirects and the background flag
    static std::array<Ast::TokenType, 10> applicableToken{
        Ast::TokenType::WORD,        Ast::TokenType::GREATER,
        Ast::TokenType::GREATER_AND, Ast::TokenType::GREATER_GREATER,
        Ast::TokenType::ONE_GREATER, Ast::TokenType::ONE_GREATER_GREATER,
        Ast::TokenType::TWO_GREATER, Ast::TokenType::TWO_GREATER_GREATER,
        Ast::TokenType::LESS,        Ast::TokenType::AMPERSAND};
    while (std::ranges::find(applicableToken, m_current_token.type) !=
           applicableToken.end())
    {

        Ast::TokenType op = m_current_token.type;
        std::string target = m_current_token.value;
        switch (m_current_token.type)
        {
        case Ast::TokenType::WORD:
            args.push_back(m_current_token.value);
            Eat(Ast::TokenType::WORD);
            break;
        case Ast::TokenType::AMPERSAND:
            background = true;
            Eat(Ast::TokenType::AMPERSAND);
            break;
        case Ast::TokenType::GREATER:
        case Ast::TokenType::LESS:
        case Ast::TokenType::GREATER_AND:
        case Ast::TokenType::GREATER_GREATER:
        case Ast::TokenType::ONE_GREATER:
        case Ast::TokenType::ONE_GREATER_GREATER:
        case Ast::TokenType::TWO_GREATER:
        case Ast::TokenType::TWO_GREATER_GREATER:
            // there is a bug concerning the GREATER_GREATER token, it is
            // missing
            Eat(op);
            // After a redirect we expected a WORD
            if (m_current_token.type != Ast::TokenType::WORD)
            {
                throw "Expected filename after redirection";
            }
            target = m_current_token.value;
            Eat(Ast::TokenType::WORD);
            redirects.push_back({.Type = op, .Target = target});
            break;
        default:
            throw "Unexpected Token encountered while parsing command";
        }
    }

    return Command{
        .Args = args, .Redirects = redirects, .Background = background};
}

void Parser::Eat(Ast::TokenType token_type)
{
    if (m_current_token.type == token_type)
    {
        m_current_token = m_lexer.NextToken();

        return;
    }

    throw std::format(
        "Expected {}, got {}",
        Ast::TokenTypeNames[static_cast<int>(token_type)],
        Ast::TokenTypeNames[static_cast<int>(m_current_token.type)]);
}

Node *Parser::Parse() { return ParseSequence(); }
