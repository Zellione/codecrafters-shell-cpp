#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

enum class ParserState : std::uint8_t {
    NORMAL,
    INSIDE_SINGLE_QUOTES,
    INSIDE_DOUBLE_QUOTES,
    ON_BACKSLASH,
    REDIRECT_STDOUT,
    REDIRECT_STDOUT_APPEND,
    REDIRECT_STDERR,
    REDIRECT_STDERR_APPEND
};

enum class TokenType : std::uint8_t {
    NORMAL,
    REDIRECT_STDOUT,
    REDIRECT_STDDERR,
    REDIRECT_STDOUT_APPEND,
    REDIRECT_STDERR_APPEND
};

struct Token {
    Token(std::string token, TokenType type)
        : token(std::move(token)), type(type) {}
    std::string token;
    TokenType type;
};

struct InternalToken {
    InternalToken() : start_pos(0), end_pos(0) {}
    InternalToken(size_t start_pos, size_t end_pos, std::string buffer)
        : start_pos(start_pos), end_pos(end_pos), buffer(std::move(buffer)) {}
    size_t start_pos;
    size_t end_pos;
    std::string buffer;
};

class TokenParser {
  private:
    [[nodiscard]] static ParserState
    DetermineState(const std::string &commandline, size_t pos,
                   ParserState current);

    [[nodiscard]] static TokenType DetermineTokenType(ParserState state);

    [[nodiscard]] static InternalToken
    ParseInsideSingleQuotes(const std::string &commandline, size_t start_pos);

    [[nodiscard]] static InternalToken
    ParseInsideDoubleQuotes(const std::string &commandline, size_t start_pos);

    [[nodiscard]] static InternalToken
    ParsePreviousBackslash(const std::string &commandline, size_t start_pos);

    [[nodiscard]] static InternalToken
    ParseRedirect(const std::string &commandline, size_t start_pos);

  public:
    TokenParser() = default;

    [[nodiscard]] static std::vector<Token>
    Parse(const std::string &commandline);
};
