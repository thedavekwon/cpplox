#include <format>
#include <memory>

#include <ast/expr.h>
#include <ast/statement.h>
#include <catch2/catch_test_macros.hpp>

using namespace cpplox;

TEST_CASE("AstPrinter Simple Binary") {
    // 1.0 + 2.0
    Expr expr{ BinaryExpr{ LiteralExpr{1.0}, { TokenType::PLUS, "+", std::nullopt, 0 }, LiteralExpr{2.0} } };
    REQUIRE(std::format("{}", expr) == "(+ 1 2)");
}

TEST_CASE("AstPrinter Complex Expression") {
    // -123 * (45.67)
    Expr expr{
        BinaryExpr{
            UnaryExpr{
                {TokenType::MINUS, "-", std::nullopt, 0},
                LiteralExpr{123.0}
            },
            {TokenType::STAR, "*", std::nullopt, 0},
            GroupingExpr{LiteralExpr{45.67}}
        }
    };
    REQUIRE(std::format("{}", expr) == "(* (- 123) (group 45.67))");
}

TEST_CASE("AstPrinter Statements") {
    // print 1 + 2;
    Statement printStmt{ PrintStatement{ BinaryExpr{ LiteralExpr{1.0}, { TokenType::PLUS, "+", std::nullopt, 0 }, LiteralExpr{2.0} } } };
    REQUIRE(std::format("{}", printStmt) == "print (+ 1 2);");

    // var a = 5 + 5;
    Statement varStmt{ VarStatement{ Token{ TokenType::IDENTIFIER, "a", std::nullopt, 0 }, BinaryExpr{ LiteralExpr{5.0}, { TokenType::PLUS, "+", std::nullopt, 0 }, LiteralExpr{5.0} } } };
    REQUIRE(std::format("{}", varStmt) == "var a = (+ 5 5);");

    // if (true) print 1; else print 2;
    Statement ifStmt{ IfStatement{ LiteralExpr{"true"}, PrintStatement{ LiteralExpr{1.0} }, PrintStatement{ LiteralExpr{2.0} } } };
    REQUIRE(std::format("{}", ifStmt) == "if (\"true\") print 1; else print 2;");
}
