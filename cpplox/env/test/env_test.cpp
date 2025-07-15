#include <string>
#include <format>
#include <memory>

#include <env/interpreter.h>
#include <env/value.h>

#include <catch2/catch_test_macros.hpp>

using namespace cpplox;

TEST_CASE("IntegerAddition") {
    Diagnostic d;
    Interpreter interpreter{ d };
    // 1.0 + 2.0
    Expr expr{ BinaryExpr{ LiteralExpr{1.0}, { TokenType::PLUS, "+", std::nullopt, 0 }, LiteralExpr{2.0} } };
    auto value = interpreter.interpretExpr(expr);
    REQUIRE(value.has_value());
    REQUIRE(std::get<double>(value.value()) == 3.0);
}

TEST_CASE("StringConcat") {
    Diagnostic d;
    Interpreter interpreter{ d };
    // "hello" + " world";
    Expr expr{ BinaryExpr{ LiteralExpr{"hello"}, { TokenType::PLUS, "+", std::nullopt, 0 }, LiteralExpr{" world"} } };
    auto value = interpreter.interpretExpr(expr);
    REQUIRE(value.has_value());
    REQUIRE(std::get<std::string>(value.value()) == "hello world");
}