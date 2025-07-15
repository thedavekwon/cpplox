#pragma once

#include <format>
#include <memory>
#include <optional>
#include <print>
#include <variant>

#include <ast/expr.h>
#include <ast/statement.h>
#include <env/env.h>
#include <env/value.h>
#include <diagnostic/diagnostic.h>

namespace cpplox {

// Tree-walk interpreter
class Interpreter {
    Value evaluate(const Expr& expr) {
        return std::visit(*this, expr);
    }

    void execute(const Statement& stmt) {
        std::visit(*this, stmt);
    }

    void error(const Token& token, std::string_view message) {
        diagnostic_.error(token.line(), message);
    }

    void checkNumberOperands(const Token& op, const Value& operand);
    void checkNumberOperands(const Token& op, const Value& left, const Value& right);

public:
    Interpreter(Diagnostic& diagnostic) : diagnostic_(diagnostic) {}

    Value operator()(const AssignExpr& expr);
    Value operator()(const BinaryExpr& expr);
    Value operator()(const GroupingExpr& expr);
    Value operator()(const LiteralExpr& expr);
    Value operator()(const UnaryExpr& expr);
    Value operator()(const VarExpr& expr);

    void operator()(const BlockStatement& stmt);
    void operator()(const PrintStatement& stmt);
    void operator()(const ExprStatement& stmt);
    void operator()(const VarStatement& stmt);

    std::optional<Value> interpretExpr(const Expr& expr) {
        try {
            Value value = evaluate(expr);
            return value;
        } catch (const RuntimeError& e) {
            return std::nullopt;
        }
    }

    void interpret(const std::vector<Statement>& statements) {
        try {
            for (const Statement& statement : statements) {
                execute(statement);
            }
        } catch (const RuntimeError& e) {
            // no-op
        }
    }

private:
    Diagnostic& diagnostic_;
    std::shared_ptr<Environment> env_ = std::make_shared<Environment>();
};

} // cpplox
