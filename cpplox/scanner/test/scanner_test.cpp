#include <scanner/scanner.h>

#include <catch2/catch_test_macros.hpp>

using namespace cpplox;

namespace {

Token t(TokenType type, std::string lexeme) {
    return Token(type, std::move(lexeme), std::nullopt, 0);
}

}

// Add Exhaustive Token
TEST_CASE("ScannerSimple") {
    Diagnostic d;
    std::string s = "1 + 1";

    Scanner scanner(s, d);
    auto tokens = scanner.scanTokens();

    std::vector<Token> gt = { t(TokenType::NUMBER, "1"), t(TokenType::PLUS, "+"), t(TokenType::NUMBER, "1"), t(TokenType::EOFF, "") };
    REQUIRE(tokens.size() == gt.size());
    for (size_t i = 0; i < tokens.size(); i++) {
        REQUIRE(tokens[i].type() == gt[i].type());
        REQUIRE(tokens[i].lexeme() == gt[i].lexeme());
    }
}

// This test covers all token types.
TEST_CASE("Exhaustive Token Scan") {
    Diagnostic d;
    std::string s = R"SRC(
// Punctuation
( ) { } , . - + ; / *

// Operators
! != = == < <= > >=

// Literals
"a string"
123.45
identifier_

// Keywords
and class else false for fun if nil or print return super this true var while
)SRC";

    Scanner scanner(s, d);
    auto tokens = scanner.scanTokens();

    std::vector<Token> gt = {
        t(TokenType::LEFT_PAREN, "("),
        t(TokenType::RIGHT_PAREN, ")"),
        t(TokenType::LEFT_BRACE, "{"),
        t(TokenType::RIGHT_BRACE, "}"),
        t(TokenType::COMMA, ","),
        t(TokenType::DOT, "."),
        t(TokenType::MINUS, "-"),
        t(TokenType::PLUS, "+"),
        t(TokenType::SEMICOLON, ";"),
        t(TokenType::SLASH, "/"),
        t(TokenType::STAR, "*"),
        t(TokenType::BANG, "!"),
        t(TokenType::BANG_EQUAL, "!="),
        t(TokenType::EQUAL, "="),
        t(TokenType::EQUAL_EQUAL, "=="),
        t(TokenType::LESS, "<"),
        t(TokenType::LESS_EQUAL, "<="),
        t(TokenType::GREATER, ">"),
        t(TokenType::GREATER_EQUAL, ">="),
        t(TokenType::STRING, "\"a string\""),
        t(TokenType::NUMBER, "123.45"),
        t(TokenType::IDENTIFIER, "identifier_"),
        t(TokenType::AND, "and"), t(TokenType::CLASS, "class"), t(TokenType::ELSE, "else"), t(TokenType::FALSE, "false"), t(TokenType::FOR, "for"), t(TokenType::FUN, "fun"), t(TokenType::IF, "if"), t(TokenType::NIL, "nil"), t(TokenType::OR, "or"), t(TokenType::PRINT, "print"), t(TokenType::RETURN, "return"), t(TokenType::SUPER, "super"), t(TokenType::THIS, "this"), t(TokenType::TRUE, "true"), t(TokenType::VAR, "var"), t(TokenType::WHILE, "while"),
        t(TokenType::EOFF, "")
    };

    REQUIRE(tokens.size() == gt.size());
    for (size_t i = 0; i < tokens.size(); i++) {
        REQUIRE(tokens[i].type() == gt[i].type());
        REQUIRE(tokens[i].lexeme() == gt[i].lexeme());
    }
}
