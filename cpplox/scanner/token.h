#pragma once

#include <print>
#include <format>
#include <optional>
#include <string>
#include <variant>

#include <magic_enum/magic_enum.hpp>

namespace cpplox {

using TokenLiteral = std::variant<double, std::string>;
using OptionalTokenLiteral = std::optional<TokenLiteral>;

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    ERROR,
    EOFF
};

class Token {
public:
    Token(TokenType type, std::string lexeme, OptionalTokenLiteral literal, int line)
        : type_(type), lexeme_(std::move(lexeme)), literal_(std::move(literal)), line_(line) {
    }

    TokenType type() const {
        return type_;
    }
    const std::string& lexeme() const {
        return lexeme_;
    }
    const OptionalTokenLiteral& literal() const {
        return literal_;
    }
    int line() const {
        return line_;
    }

    bool operator==(const Token& other) const {
        return type_ == other.type_ && lexeme_ == other.lexeme_ && literal_ == other.literal_ && line_ == other.line_;
    }

private:
    TokenType type_;
    std::string lexeme_;
    OptionalTokenLiteral literal_;
    int line_;

    friend class std::formatter<Token>;
};

} // cpplox

template <>
struct std::formatter<cpplox::TokenType> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::TokenType& t, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}", magic_enum::enum_name(t));
    }
};

template <>
struct std::formatter<cpplox::Token> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::Token& t, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{} {}", t.type(), t.lexeme());
    }
};