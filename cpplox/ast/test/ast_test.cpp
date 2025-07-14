#include <format>
#include <memory>

#include <ast/expr.h>

#include <catch2/catch_test_macros.hpp>

using namespace cpplox;

TEST_CASE("AstPrinter") {
    // 1.0 + 2.0
    Expr expr{ BinaryExpr{ LiteralExpr{1.0}, { TokenType::PLUS, "+", std::nullopt, 0 }, LiteralExpr{2.0} } };
    REQUIRE(std::format("{}", expr) == "(+ 1 2)");
}
