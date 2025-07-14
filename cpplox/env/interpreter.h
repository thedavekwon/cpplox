#pragma once

#include <format>
#include <optional>
#include <print>
#include <variant>

#include <ast/expr.h>
#include <env/value.h>
#include <diagnostic/diagnostic.h>

namespace cpplox {

// Tree-walk interpreter
class Interpreter {
    Value evaluate(const Expr& expr) {
        return std::visit(*this, expr);
    }

    void error(const Token& token, std::string_view message) {
        diagnostic_.error(token.line(), message);
    }

    class RuntimeError : public std::runtime_error {
    public:
        RuntimeError() : std::runtime_error("Runtime error") {}
    };

    void checkNumberOperands(const Token& op, const Value& operand);
    void checkNumberOperands(const Token& op, const Value& left, const Value& right);

public:
    Interpreter(Diagnostic& diagnostic) : diagnostic_(diagnostic) {}

    Value operator()(const BinaryExpr& expr);
    Value operator()(const GroupingExpr& expr);
    Value operator()(const LiteralExpr& expr);
    Value operator()(const UnaryExpr& expr);

    std::optional<Value> interpret(const Expr& expr) {
        try {
            Value value = evaluate(expr);
            return value;
        }
        catch (const RuntimeError& e) {
            return std::nullopt;
        }
    }

private:
    Diagnostic& diagnostic_;
};

} // cpplox
