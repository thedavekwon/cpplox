#include <env/interpreter.h>

#include <variant> 

#include <ast/expr.h>
#include <env/value.h>
#include <util/scope_guard.h>

namespace {

bool isTruthy(const cpplox::Value& value) {
    return std::visit([]<typename V>(const V & v) {
        if constexpr (std::is_same_v<V, std::nullptr_t>) {
            return false;
        } else if constexpr (std::is_same_v<V, bool>) {
            return v;
        }
        return true;
    }, value);
}

bool isEqual(const cpplox::Value& l, const cpplox::Value& r) {
    return std::visit([](const auto& l, const auto& r) {
        if constexpr (std::is_same_v<decltype(l), decltype(r)>) {
            return l == r;
        } else {
            return false;
        }
        }, l, r);
}

}

namespace cpplox {

Value Interpreter::operator()(const AssignExpr& expr) {
    Value value = evaluate(*expr.value);
    env_->assign(expr.name, std::move(value));
    return value;
}

Value Interpreter::operator()(const BinaryExpr& expr) {
    Value left = evaluate(*expr.left);
    Value right = evaluate(*expr.right);

    switch (expr.op.type()) {
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL:
            return isEqual(left, right);
        case TokenType::GREATER:
            checkNumberOperands(expr.op, left, right);
            return std::get<double>(left) > std::get<double>(right);
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.op, left, right);
            return std::get<double>(left) >= std::get<double>(right);
        case TokenType::LESS:
            checkNumberOperands(expr.op, left, right);
            return std::get<double>(left) < std::get<double>(right);
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.op, left, right);
            return std::get<double>(left) <= std::get<double>(right);
        case TokenType::MINUS:
            checkNumberOperands(expr.op, left, right);
            return std::get<double>(left) - std::get<double>(right);
        case TokenType::SLASH:
            checkNumberOperands(expr.op, left, right);
            return std::get<double>(left) / std::get<double>(right);
        case TokenType::STAR:
            checkNumberOperands(expr.op, left, right);
            return std::get<double>(left) * std::get<double>(right);
        case TokenType::PLUS: {
            // operator+ is overloaded for numbers and strings
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                return std::get<double>(left) + std::get<double>(right);
            }
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }
            error(expr.op, "Operands must be two numbers or two strings.");
            throw RuntimeError();
        }
        default:
            // Unreachable.
            break;
    }
    std::unreachable();
}

Value Interpreter::operator()(const GroupingExpr& expr) {
    return evaluate(*expr.expr);
}

Value Interpreter::operator()(const LiteralExpr& expr) {
    if (expr.value.has_value()) {
        return  std::visit([](const auto& l) {return Value{ l };}, *expr.value);
    }
    return nullptr;
}

Value Interpreter::operator()(const LogicalExpr& expr) {
    Value left = evaluate(*expr.left);

    if (expr.op.type() == TokenType::OR) {
        if (isTruthy(left)) { return left; }
    } else { if (!isTruthy(left)) { return left; } }

    return evaluate(*expr.right);
}

Value Interpreter::operator()(const UnaryExpr& expr) {
    Value right = evaluate(*expr.right);
    if (expr.op.type() == TokenType::BANG) {
        return !isTruthy(right);
    } else if (expr.op.type() == TokenType::MINUS) {
        checkNumberOperands(expr.op, right);
        return -std::get<double>(right);
    }
    std::unreachable();
}

Value Interpreter::operator()(const VarExpr& expr) {
    return env_->get(expr.name);
}

void Interpreter::operator()(const BlockStatement& stmt) {
    ScopeGuard guard{ [this, env = env_]() {
        env_ = env;
    } };

    env_ = std::make_shared<Environment>(env_);
    for (const Statement& statement : stmt.statements) {
        execute(statement);
    }
}

void Interpreter::operator()(const ExprStatement& stmt) {
    evaluate(stmt.expr);
}

void Interpreter::operator()(const IfStatement& stmt) {
    if (isTruthy(evaluate(stmt.condition))) {
        execute(*stmt.thenBranch);
    } else if (stmt.elseBranch) {
        execute(*stmt.elseBranch);
    }
}

void Interpreter::operator()(const PrintStatement& stmt) {
    Value value = evaluate(stmt.expr);
    std::print("{}\n", value);
}

void Interpreter::operator()(const VarStatement& stmt) {
    Value value;
    if (stmt.initializer.has_value()) {
        value = evaluate(*stmt.initializer);
    }
    env_->define(stmt.name.lexeme(), std::move(value));
}

void Interpreter::operator()(const WhileStatement& stmt) {
    while (isTruthy(evaluate(stmt.condition))) {
        execute(*stmt.body);
    }
}

void Interpreter::checkNumberOperands(const Token& op, const Value& operand) {
    if (std::holds_alternative<double>(operand)) {
        return;
    }
    error(op, "Operands must be a number.");
    throw RuntimeError();
}

void Interpreter::checkNumberOperands(const Token& op, const Value& left, const Value& right) {
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
        return;
    }
    error(op, "Operands must be numbers.");
    throw RuntimeError();
}


} // cpplox