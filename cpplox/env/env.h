#pragma once

#include <unordered_map>

#include <diagnostic/diagnostic.h>
#include <env/value.h>
#include <scanner/token.h>

namespace cpplox {

class RuntimeError : public std::runtime_error {
public:
    RuntimeError() : std::runtime_error("Runtime error") {}
};

class Environment {
public:
    Environment() = default;
    Environment(std::shared_ptr<Environment> enclosing) : enclosing_(std::move(enclosing)) {}

    // Allows redefinition
    void define(std::string name, Value value) {
        values_[std::move(name)] = std::move(value);
    }

    Value& get(const Token& name) {
        if (auto it = values_.find(name.lexeme()); it != values_.end()) {
            return it->second;
        }
        // chain
        if (enclosing_) {
            return enclosing_->get(name);
        }
        diagnostic_.error(name.line(), "Undefined variable '" + name.lexeme() + "'.");
        throw RuntimeError();
    }

    void assign(const Token& name, Value value) {
        if (values_.contains(name.lexeme())) {
            values_[name.lexeme()] = std::move(value);
            return;
        }
        // chain
        if (enclosing_) {
            enclosing_->assign(name, std::move(value));
            return;
        }
        diagnostic_.error(name.line(), "Undefined variable '" + name.lexeme() + "'.");
        throw RuntimeError();
    }

    void print() {
        std::print("{}\n", values_);
        if (enclosing_) {
            enclosing_->print();
        }
    }

private:
    std::unordered_map<std::string, Value> values_;
    Diagnostic diagnostic_;
    std::shared_ptr<Environment> enclosing_ = nullptr;
};

} // cpplox