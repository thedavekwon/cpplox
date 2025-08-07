#pragma once

#include <chrono>
#include <format>
#include <memory>
#include <optional>
#include <print>
#include <unordered_map>
#include <variant>
#include <iostream>

#include <ast/expr.h>
#include <ast/statement.h>
#include <env/env.h>
#include <env/fwd.h>
#include <env/object.h>
#include <diagnostic/diagnostic.h>

namespace cpplox {

// Tree-walk interpreter
class Interpreter {
    Object evaluate(const Expr& expr) {
        return std::visit(*this, expr);
    }

    std::optional<Object> execute(const Statement& stmt) {
        return std::visit(*this, stmt);
    }

    void checkNumberOperands(const Token& op, const Object& operand);
    void checkNumberOperands(const Token& op, const Object& left, const Object& right);
    template <typename T> requires is_contained_in_v<T, Expr>
    Object lookUpVariable(const Token& name, const T& expr) {
        if (auto it = locals_.find(&expr); it != locals_.end()) {
            return env_->getAt(it->second, name);
        } else {
            return globals_.get(name);
        }
    }

public:
    Interpreter(Diagnostic& diagnostic, std::ostream& out = std::cout);

    Object operator()(const AssignExpr& expr);
    Object operator()(const BinaryExpr& expr);
    Object operator()(const CallExpr& expr);
    Object operator()(const GetExpr& expr);
    Object operator()(const GroupingExpr& expr);
    Object operator()(const LiteralExpr& expr);
    Object operator()(const LogicalExpr& expr);
    Object operator()(const SetExpr& expr);
    Object operator()(const ThisExpr& expr);
    Object operator()(const UnaryExpr& expr);
    Object operator()(const VarExpr& expr);

    std::optional<Object> operator()(const BlockStatement& stmt, EnvironmentPtr closure = nullptr);;
    std::optional<Object> operator()(const ClassStatement& stmt);
    std::optional<Object> operator()(const ExprStatement& stmt);
    std::optional<Object> operator()(const FunctionStatement& stmt);
    std::optional<Object> operator()(const IfStatement& stmt);
    std::optional<Object> operator()(const PrintStatement& stmt);
    std::optional<Object> operator()(const ReturnStatement& stmt);
    std::optional<Object> operator()(const VarStatement& stmt);
    std::optional<Object> operator()(const WhileStatement& stmt);

    std::optional<Object> interpretExpr(const Expr& expr) {
        try {
            Object object = evaluate(expr);
            return object;
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

    void error(const Token& token, std::string_view message) {
        diagnostic_.error(token.line(), message);
    }

    template <typename T> requires is_contained_in_v<T, Expr>
    void resolve(const T& expr, size_t depth) {
        locals_[&expr] = depth;
    }

private:
    Diagnostic& diagnostic_;
    std::ostream& out_;
    static Environment globals_;
    EnvironmentPtr env_ = std::make_shared<Environment>(EnvironmentPtr(&globals_, [](Environment*) {}));
    std::unordered_map<const void*, size_t> locals_;
};

} // cpplox
