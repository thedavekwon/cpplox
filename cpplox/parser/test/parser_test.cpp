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
    auto expr = parser.parseExpr();
    REQUIRE(expr.has_value());
    const auto& binaryExpr = std::get<BinaryExpr>(*expr);
    const auto& left = std::get<LiteralExpr>(*binaryExpr.left);
    REQUIRE(binaryExpr.op.type() == TokenType::PLUS);
    const auto& right = std::get<LiteralExpr>(*binaryExpr.right);
    REQUIRE(std::get<double>(*left.object) == 1.0);
    REQUIRE(std::get<double>(*right.object) == 2.0);
}

TEST_CASE("ParserLiterals") {
    Diagnostic d;

    // true
    std::vector<Token> tokens_true = { t(TokenType::TRUE, "true"), t(TokenType::EOFF, "") };
    Parser parser_true(tokens_true, d);
    auto expr_true = parser_true.parseExpr();
    REQUIRE(expr_true.has_value());
    const auto& lit_true = std::get<LiteralExpr>(*expr_true);
    REQUIRE(std::get<std::string>(*lit_true.object) == "true");

    // "a string"
    std::vector<Token> tokens_str = { t(TokenType::STRING, "\"a string\"", std::string("a string")), t(TokenType::EOFF, "") };
    Parser parser_str(tokens_str, d);
    auto expr_str = parser_str.parseExpr();
    REQUIRE(expr_str.has_value());
    const auto& lit_str = std::get<LiteralExpr>(*expr_str);
    REQUIRE(std::get<std::string>(*lit_str.object) == "a string");

    // nil
    std::vector<Token> tokens_nil = { t(TokenType::NIL, "nil"), t(TokenType::EOFF, "") };
    Parser parser_nil(tokens_nil, d);
    auto expr_nil = parser_nil.parseExpr();
    REQUIRE(expr_nil.has_value());
    const auto& lit_nil = std::get<LiteralExpr>(*expr_nil);
    REQUIRE(std::get<std::string>(*lit_nil.object) == "nil");
}

TEST_CASE("ParserExpressionPrecedence") {
    Diagnostic d;
    // -a * b + c / d < e == f
    // should be parsed as: (((-a) * b) + (c / d)) < e) == f
    std::vector<Token> tokens = {
        t(TokenType::MINUS, "-"), t(TokenType::IDENTIFIER, "a"), t(TokenType::STAR, "*"), t(TokenType::IDENTIFIER, "b"),
        t(TokenType::PLUS, "+"), t(TokenType::IDENTIFIER, "c"), t(TokenType::SLASH, "/"), t(TokenType::IDENTIFIER, "d"),
        t(TokenType::LESS, "<"), t(TokenType::IDENTIFIER, "e"),
        t(TokenType::EQUAL_EQUAL, "=="), t(TokenType::IDENTIFIER, "f"),
        t(TokenType::EOFF, "")
    };

    Parser parser(tokens, d);
    auto expr = parser.parseExpr();
    REQUIRE(expr.has_value());

    // Outermost: ==
    const auto& eq_expr = std::get<BinaryExpr>(*expr);
    REQUIRE(eq_expr.op.type() == TokenType::EQUAL_EQUAL);
    REQUIRE(std::get<VarExpr>(*eq_expr.right).name.lexeme() == "f");

    // Next: <
    const auto& less_expr = std::get<BinaryExpr>(*eq_expr.left);
    REQUIRE(less_expr.op.type() == TokenType::LESS);
    REQUIRE(std::get<VarExpr>(*less_expr.right).name.lexeme() == "e");

    // Next: +
    const auto& plus_expr = std::get<BinaryExpr>(*less_expr.left);
    REQUIRE(plus_expr.op.type() == TokenType::PLUS);

    // Left of + is (-a * b)
    const auto& mul_expr = std::get<BinaryExpr>(*plus_expr.left);
    REQUIRE(mul_expr.op.type() == TokenType::STAR);
    REQUIRE(std::get<VarExpr>(*mul_expr.right).name.lexeme() == "b");
    const auto& unary_expr = std::get<UnaryExpr>(*mul_expr.left);
    REQUIRE(unary_expr.op.type() == TokenType::MINUS);
    REQUIRE(std::get<VarExpr>(*unary_expr.right).name.lexeme() == "a");

    // Right of + is (c / d)
    const auto& div_expr = std::get<BinaryExpr>(*plus_expr.right);
    REQUIRE(div_expr.op.type() == TokenType::SLASH);
    REQUIRE(std::get<VarExpr>(*div_expr.left).name.lexeme() == "c");
    REQUIRE(std::get<VarExpr>(*div_expr.right).name.lexeme() == "d");
}

TEST_CASE("ParserGrouping") {
    Diagnostic d;
    std::vector<Token> tokens = { t(TokenType::NUMBER, "1", 1.0), t(TokenType::STAR, "*"), t(TokenType::LEFT_PAREN, "("), t(TokenType::NUMBER, "2", 2.0), t(TokenType::PLUS, "+"), t(TokenType::NUMBER, "3", 3.0), t(TokenType::RIGHT_PAREN, ")"), t(TokenType::EOFF, "") };
    Parser parser(tokens, d);
    auto expr = parser.parseExpr();
    REQUIRE(expr.has_value());
    const auto& mul_expr = std::get<BinaryExpr>(*expr);
    REQUIRE(mul_expr.op.type() == TokenType::STAR);
    REQUIRE(std::get<double>(*std::get<LiteralExpr>(*mul_expr.left).object) == 1.0);
    const auto& group_expr = std::get<GroupingExpr>(*mul_expr.right);
    const auto& plus_expr = std::get<BinaryExpr>(*group_expr.expr);
    REQUIRE(plus_expr.op.type() == TokenType::PLUS);
}

TEST_CASE("ParserAssignment") {
    Diagnostic d;
    // a = b = 10
    // should be parsed as a = (b = 10) due to right-associativity
    std::vector<Token> tokens = {
        t(TokenType::IDENTIFIER, "a"), t(TokenType::EQUAL, "="),
        t(TokenType::IDENTIFIER, "b"), t(TokenType::EQUAL, "="),
        t(TokenType::NUMBER, "10", 10.0),
        t(TokenType::EOFF, "")
    };

    Parser parser(tokens, d);
    auto expr = parser.parseExpr();
    REQUIRE(expr.has_value());

    // Outermost: a = ...
    const auto& assign_a = std::get<AssignExpr>(*expr);
    REQUIRE(assign_a.name.lexeme() == "a");

    // Right-hand side of 'a' assignment: b = 10
    const auto& assign_b = std::get<AssignExpr>(*assign_a.object);
    REQUIRE(assign_b.name.lexeme() == "b");

    // Right-hand side of 'b' assignment: 10
    const auto& literal = std::get<LiteralExpr>(*assign_b.object);
    REQUIRE(std::get<double>(*literal.object) == 10.0);
}

TEST_CASE("ParserComplexExpressionCombination") {
    Diagnostic d;
    // 5 * (2 - 1) + 3 < 10 == !false
    // should be parsed as: ((((5 * (group (2 - 1))) + 3) < 10) == (!false))
    std::vector<Token> tokens = {
        t(TokenType::NUMBER, "5", 5.0), t(TokenType::STAR, "*"),
        t(TokenType::LEFT_PAREN, "("),
        t(TokenType::NUMBER, "2", 2.0), t(TokenType::MINUS, "-"), t(TokenType::NUMBER, "1", 1.0),
        t(TokenType::RIGHT_PAREN, ")"),
        t(TokenType::PLUS, "+"), t(TokenType::NUMBER, "3", 3.0),
        t(TokenType::LESS, "<"), t(TokenType::NUMBER, "10", 10.0),
        t(TokenType::EQUAL_EQUAL, "=="),
        t(TokenType::BANG, "!"), t(TokenType::FALSE, "false"),
        t(TokenType::EOFF, "")
    };

    Parser parser(tokens, d);
    auto expr = parser.parseExpr();
    REQUIRE(expr.has_value());

    // Walk the AST to verify precedence and structure
    const auto& eq_expr = std::get<BinaryExpr>(*expr);
    REQUIRE(eq_expr.op.type() == TokenType::EQUAL_EQUAL);

    const auto& unary_false = std::get<UnaryExpr>(*eq_expr.right);
    REQUIRE(unary_false.op.type() == TokenType::BANG);

    const auto& less_expr = std::get<BinaryExpr>(*eq_expr.left);
    REQUIRE(less_expr.op.type() == TokenType::LESS);

    const auto& plus_expr = std::get<BinaryExpr>(*less_expr.left);
    REQUIRE(plus_expr.op.type() == TokenType::PLUS);

    const auto& mul_expr = std::get<BinaryExpr>(*plus_expr.left);
    REQUIRE(mul_expr.op.type() == TokenType::STAR);

    const auto& group_expr = std::get<GroupingExpr>(*mul_expr.right);
    REQUIRE(std::holds_alternative<BinaryExpr>(*group_expr.expr));
}