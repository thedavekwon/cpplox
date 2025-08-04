#include <variant>

#include <env/resolver.h>

namespace cpplox {


void Resolver::operator()(const AssignExpr& expr) {
    resolve(*expr.object);
    resolveLocal(expr, expr.name);
}

void Resolver::operator()(const BinaryExpr& expr) {
    resolve(*expr.left);
    resolve(*expr.right);
}

void Resolver::operator()(const CallExpr& expr) {
    resolve(*expr.callee);
    for (const auto& argument : expr.arguments) {
        resolve(argument);
    }
}

void Resolver::operator()(const GroupingExpr& expr) {
    resolve(*expr.expr);
}

void Resolver::operator()(const LiteralExpr& expr) {
    // no-op
}

void Resolver::operator()(const LogicalExpr& expr) {
    resolve(*expr.left);
    resolve(*expr.right);
}

void Resolver::operator()(const UnaryExpr& expr) {
    resolve(*expr.right);
}

void Resolver::operator()(const VarExpr& expr) {
    if (!scopes_.empty()) {
        if (auto it = scopes_.back().find(expr.name.lexeme()); it != scopes_.back().end()) {
            if (it->second == false) {
                interpreter_.error(expr.name, "Can't read local variable in its own initializer.");
            }
        }
    }
    resolveLocal(expr, expr.name);
}

void Resolver::operator()(const BlockStatement& stmt) {
    resolve(stmt.statements);
}

void Resolver::operator()(const ExprStatement& stmt) {
    resolve(stmt.expr);
}

void Resolver::operator()(const FunctionStatement& stmt) {
    declare(stmt.name);
    define(stmt.name);
    resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::operator()(const IfStatement& stmt) {
    resolve(stmt.condition);
    resolve(*stmt.thenBranch);
    if (stmt.elseBranch) {
        resolve(*stmt.elseBranch);
    }
}

void Resolver::operator()(const PrintStatement& stmt) {
    resolve(stmt.expr);
}

void Resolver::operator()(const ReturnStatement& stmt) {
    if (currentFunction_ == FunctionType::None) {
        interpreter_.error(stmt.keyword, "Can't return from top-level code.");
    }

    if (stmt.value.has_value()) {
        resolve(*stmt.value);
    }
}

void Resolver::operator()(const VarStatement& stmt) {
    declare(stmt.name);
    if (stmt.initializer.has_value()) {
        resolve(*stmt.initializer);
    }
    define(stmt.name);
}

void Resolver::operator()(const WhileStatement& stmt) {
    resolve(stmt.condition);
    resolve(stmt.body->statements);
}

void Resolver::resolve(const Expr& expr) {
    std::visit(*this, expr);
}

void Resolver::resolve(const Statement& stmt) {
    std::visit(*this, stmt);
}

void Resolver::resolve(const std::vector<Statement>& stmts, bool newScope) {
    if (newScope) beginScope();
    for (const auto& stmt : stmts) {
        resolve(stmt);
    }
    if (newScope) endScope();
}

void Resolver::resolveFunction(const FunctionStatement& stmt, FunctionType funcType) {
    FunctionType enclosingFunction = currentFunction_;
    currentFunction_ = funcType;

    beginScope();
    for (const auto& param : stmt.params) {
        declare(param);
        define(param);
    }
    resolve(stmt.body->statements);
    endScope();

    currentFunction_ = enclosingFunction;
}

void Resolver::beginScope() {
    scopes_.push_back({});
}

void Resolver::endScope() {
    scopes_.pop_back();
}

void Resolver::declare(const Token& name) {
    if (scopes_.empty()) return;
    if (scopes_.back().contains(name.lexeme())) {
        interpreter_.error(name, "Already a variable with this name in this scope.");
    }

    scopes_.back()[name.lexeme()] = false;
}

void Resolver::define(const Token& name) {
    if (scopes_.empty()) return;
    scopes_.back()[name.lexeme()] = true;
}

} // cpplox