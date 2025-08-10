#include <variant>

#include <env/resolver.h>
#include <util/scope_guard.h>

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

void Resolver::operator()(const GetExpr& expr) {
    resolve(*expr.object);
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

void Resolver::operator()(const SetExpr& expr) {
    resolve(*expr.object);
    resolve(*expr.value);
}

void Resolver::operator()(const SuperExpr& expr) {
    if (currentClass_ == ClassType::None) {
        interpreter_.error(expr.keyword, "Can't use 'super' outside of a class.");
    } else if (currentClass_ != ClassType::SUBCLASS) {
        interpreter_.error(expr.keyword, "Can't use 'super' in a class with no superclass.");
    }
    resolveLocal(expr, expr.keyword);
}

void Resolver::operator()(const ThisExpr& expr) {
    if (currentClass_ == ClassType::None) {
        interpreter_.error(expr.keyword, "Can't use 'this' outside of a class.");
        return;
    }
    resolveLocal(expr, expr.keyword);
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

void Resolver::operator()(const ClassStatement& stmt) {
    ScopeGuard guard{ [this, enclosingClass = std::exchange(currentClass_, ClassType::CLASS)]() {
        currentClass_ = enclosingClass;
    } };

    declare(stmt.name);
    define(stmt.name);

    if (stmt.superclass.has_value()) {
        if (stmt.superclass->name.lexeme() == stmt.name.lexeme()) {
            interpreter_.error(stmt.superclass->name, "A class can't inherit from itself.");
        }
        currentClass_ = ClassType::SUBCLASS;
        operator()(*stmt.superclass);

        beginScope();
        scopes_.back()["super"] = true;
    }

    beginScope();
    scopes_.back()["this"] = true;
    for (const auto& method : stmt.methods) {
        resolveFunction(method, method.name.lexeme() == "init" ? FunctionType::INITIALIZER : FunctionType::METHOD);
    }
    endScope();

    if (stmt.superclass.has_value()) {
        endScope();
    }
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

    if (!stmt.value.has_value()) {
        return;
    }

    if (currentFunction_ == FunctionType::INITIALIZER) {
        interpreter_.error(stmt.keyword, "Can't return a value from an initializer.");
    }

    resolve(*stmt.value);
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
    ScopeGuard guard{ [this, enclosingFunction = std::exchange(currentFunction_, funcType)]() {
        currentFunction_ = enclosingFunction;
    } };

    beginScope();
    for (const auto& param : stmt.params) {
        declare(param);
        define(param);
    }
    resolve(stmt.body->statements);
    endScope();
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