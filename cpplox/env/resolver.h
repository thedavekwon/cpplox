#pragma once

#include <unordered_map>
#include <vector>

#include <ast/expr.h>
#include <ast/statement.h>
#include <env/fwd.h>
#include <env/interpreter.h>
#include <util/traits.h>

namespace cpplox {

class Resolver {
private:
    enum class FunctionType {
        None,
        FUNCTION,
        METHOD,
        INITIALIZER,
    };

    enum class ClassType {
        None,
        CLASS,
        SUBCLASS,
    };

public:
    Resolver(Interpreter& interpreter) : interpreter_(interpreter) {}

    void operator()(const AssignExpr& expr);
    void operator()(const BinaryExpr& expr);
    void operator()(const CallExpr& expr);
    void operator()(const GetExpr& expr);
    void operator()(const GroupingExpr& expr);
    void operator()(const LiteralExpr& expr);
    void operator()(const LogicalExpr& expr);
    void operator()(const SetExpr& expr);
    void operator()(const SuperExpr& expr);
    void operator()(const ThisExpr& expr);
    void operator()(const UnaryExpr& expr);
    void operator()(const VarExpr& expr);

    void operator()(const BlockStatement& stmt);
    void operator()(const ClassStatement& stmt);
    void operator()(const ExprStatement& stmt);
    void operator()(const FunctionStatement& stmt);
    void operator()(const IfStatement& stmt);
    void operator()(const PrintStatement& stmt);
    void operator()(const ReturnStatement& stmt);
    void operator()(const VarStatement& stmt);
    void operator()(const WhileStatement& stmt);

    void resolve(const std::vector<Statement>& stmts, bool newScope = true);
private:
    void resolve(const Expr& expr);
    void resolve(const Statement& stmt);

    template <typename T> requires is_contained_in_v<T, Expr>
    void resolveLocal(const T& expr, const Token& name) {
        for (int i = scopes_.size() - 1; i >= 0; i--) {
            if (scopes_[i].contains(name.lexeme())) {
                interpreter_.resolve(expr, scopes_.size() - 1 - i);
                return;
            }
        }
    }

    void resolveFunction(const FunctionStatement& stmt, FunctionType funcType);

    void scopeGuard();
    void beginScope();
    void endScope();

    void declare(const Token& name);
    void define(const Token& name);

    std::vector<std::unordered_map<std::string, bool>> scopes_;
    FunctionType currentFunction_ = FunctionType::None;
    ClassType currentClass_ = ClassType::None;
    Interpreter& interpreter_;
};

}