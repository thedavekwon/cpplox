#include <env/interpreter.h>

#include <variant> 

#include <ast/expr.h>
#include <env/object.h>
#include <util/scope_guard.h>

namespace {

bool isTruthy(const cpplox::Object& object) {
    return std::visit([]<typename V>(const V & v) {
        if constexpr (std::is_same_v<V, std::nullptr_t>) {
            return false;
        } else if constexpr (std::is_same_v<V, bool>) {
            return v;
        }
        return true;
    }, object);
}

bool isEqual(const cpplox::Object& l, const cpplox::Object& r) {
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

Environment Interpreter::globals_ = {};

Object Interpreter::operator()(const AssignExpr& expr) {
    Object object = evaluate(*expr.object);

    if (auto it = locals_.find(&expr); it != locals_.end()) {
        env_->assignAt(it->second, expr.name, object);
        return object;
    } else {
        globals_.assign(expr.name, object);
        return object;
    }
    return object;
}

Object Interpreter::operator()(const BinaryExpr& expr) {
    Object left = evaluate(*expr.left);
    Object right = evaluate(*expr.right);

    switch (expr.op.type()) {
        case TokenType::BANG_EQUAL:
            return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL:
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

Object Interpreter::operator()(const CallExpr& expr) {
    Object callee = evaluate(*expr.callee);

    std::vector<Object> arguments;
    for (const Expr& argument : expr.arguments) {
        arguments.push_back(evaluate(argument));
    }

    return std::visit([&]<typename T>(const T & v) -> Object {
        if constexpr (std::is_same_v<T, NativeFunctionPtr>) {
            if (arguments.size() != v->arity()) {
                error(expr.paren, std::format("Expected {} arguments but got {}.", v->arity(), arguments.size()));
                throw RuntimeError();
            }
            return v->call(this, std::move(arguments));
        } else if constexpr (std::is_same_v<T, FunctionPtr>) {
            if (arguments.size() != v->arity()) {
                error(expr.paren, std::format("Expected {} arguments but got {}.", v->arity(), arguments.size()));
                throw RuntimeError();
            }
            EnvironmentPtr env = std::make_shared<Environment>(v->closure_);
            for (size_t i = 0; i < v->arity(); i++) {
                env->define(v->declaration_.params[i].lexeme(), std::move(arguments[i]));
            }
            auto ret = operator()(*v->declaration_.body, env);
            if (ret.has_value()) {
                return ret.value();
            }
            return nullptr;
        } else {
            error(expr.paren, "Can only call functions.");
            throw RuntimeError();
        }
    }, callee);
    // Check arity
}

Object Interpreter::operator()(const GroupingExpr& expr) {
    return evaluate(*expr.expr);
}

Object Interpreter::operator()(const LiteralExpr& expr) {
    if (expr.object.has_value()) {
        return  std::visit([](const auto& l) {return Object{ l };}, *expr.object);
    }
    return nullptr;
}

Object Interpreter::operator()(const LogicalExpr& expr) {
    Object left = evaluate(*expr.left);

    if (expr.op.type() == TokenType::OR) {
        if (isTruthy(left)) { return left; }
    } else { if (!isTruthy(left)) { return left; } }

    return evaluate(*expr.right);
}

Object Interpreter::operator()(const UnaryExpr& expr) {
    Object right = evaluate(*expr.right);
    if (expr.op.type() == TokenType::BANG) {
        return !isTruthy(right);
    } else if (expr.op.type() == TokenType::MINUS) {
        checkNumberOperands(expr.op, right);
        return -std::get<double>(right);
    }
    std::unreachable();
}

Object Interpreter::operator()(const VarExpr& expr) {
    return lookUpVariable(expr.name, expr);
}

std::optional<Object> Interpreter::operator()(const BlockStatement& stmt, EnvironmentPtr env) {
    EnvironmentPtr blockEnvironment = env ? std::make_shared<Environment>(env) : std::make_shared<Environment>(env_);
    ScopeGuard guard{ [this, oldEnvironment = std::exchange(env_, blockEnvironment)]() {
        env_ = oldEnvironment;
    } };

    for (const Statement& statement : stmt.statements) {
        if (auto ret = execute(statement)) {
            return ret;
        }
    }
    return std::nullopt;
}

std::optional<Object> Interpreter::operator()(const ExprStatement& stmt) {
    evaluate(stmt.expr);
    return std::nullopt;
}

std::optional<Object> Interpreter::operator()(const FunctionStatement& stmt) {
    env_->define(stmt.name.lexeme(), std::make_shared<Function>(env_, stmt));
    return std::nullopt;
}

std::optional<Object> Interpreter::operator()(const IfStatement& stmt) {
    if (isTruthy(evaluate(stmt.condition))) {
        return execute(*stmt.thenBranch);
    } else if (stmt.elseBranch) {
        return execute(*stmt.elseBranch);
    }
    return std::nullopt;
}

std::optional<Object> Interpreter::operator()(const PrintStatement& stmt) {
    Object object = evaluate(stmt.expr);
    std::print("{}\n", object);
    return std::nullopt;
}

std::optional<Object> Interpreter::operator()(const ReturnStatement& stmt) {
    std::optional<Object> value;
    if (stmt.value.has_value()) {
        value = evaluate(*stmt.value);
    }
    return value;
}

std::optional<Object> Interpreter::operator()(const VarStatement& stmt) {
    Object object;
    if (stmt.initializer.has_value()) {
        object = evaluate(*stmt.initializer);
    }
    env_->define(stmt.name.lexeme(), std::move(object));
    return std::nullopt;
}

std::optional<Object> Interpreter::operator()(const WhileStatement& stmt) {
    while (isTruthy(evaluate(stmt.condition))) {
        auto ret = operator()(*stmt.body);
        if (ret.has_value()) {
            return ret;
        }
    }
    return std::nullopt;
}

void Interpreter::checkNumberOperands(const Token& op, const Object& operand) {
    if (std::holds_alternative<double>(operand)) {
        return;
    }
    error(op, "Operands must be a number.");
    throw RuntimeError();
}

void Interpreter::checkNumberOperands(const Token& op, const Object& left, const Object& right) {
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
        return;
    }
    error(op, "Operands must be numbers.");
    throw RuntimeError();
}


} // cpplox