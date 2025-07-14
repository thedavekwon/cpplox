#include <parser/parser.h>

#include <catch2/catch_test_macros.hpp>

using namespace cpplox;

namespace {

Token t(TokenType type, std::string lexeme, OptionalTokenLiteral literal = std::nullopt) {
    return Token(type, std::move(lexeme), std::move(literal), 0);
}

}

TEST_CASE("ParserSimple") {
    Diagnostic d;
    std::vector<Token> tokens = { t(TokenType::NUMBER, "1", 1.0), t(TokenType::PLUS, "+"), t(TokenType::NUMBER, "2", 2.0), t(TokenType::EOFF, "") };

    Parser parser(tokens, d);
    auto expr = parser.parse();
    REQUIRE(expr.has_value());
    const auto& binaryExpr = std::get<BinaryExpr>(*expr);
    const auto& left = std::get<LiteralExpr>(*binaryExpr.left);
    const auto& right = std::get<LiteralExpr>(*binaryExpr.right);
    REQUIRE(std::get<double>(*left.value) == 1.0);
    REQUIRE(std::get<double>(*right.value) == 2.0);
}
