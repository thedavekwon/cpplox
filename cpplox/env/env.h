#pragma once

#include <memory>
#include <unordered_map>

#include <diagnostic/diagnostic.h>
#include <env/object.h>
#include <scanner/token.h>

namespace cpplox {

class RuntimeError : public std::runtime_error {
public:
    RuntimeError() : std::runtime_error("Runtime error") {}
};

using EnvironmentPtr = std::shared_ptr<class Environment>;
class Environment : public std::enable_shared_from_this<Environment> {
public:
    Environment() = default;
    Environment(EnvironmentPtr enclosing) : enclosing_(std::move(enclosing)) {}

    // Allows redefinition
    void define(std::string name, Object object) {
        objects_[std::move(name)] = std::move(object);
    }

    Object& get(const Token& name) {
        if (auto it = objects_.find(name.lexeme()); it != objects_.end()) {
            return it->second;
        }
        // chain
        if (enclosing_) {
            return enclosing_->get(name);
        }
        diagnostic_.error(name.line(), "Undefined variable '" + name.lexeme() + "'.");
        throw RuntimeError();
    }

    Object& getAt(int distance, const std::string& name) {
        return ancestor(distance)->objects_[name];
    }

    EnvironmentPtr ancestor(int distance) {
        EnvironmentPtr env = shared_from_this();
        for (int i = 0; i < distance; i++) {
            env = env->enclosing_;
        }
        return env;
    }

    void assign(const Token& name, Object object) {
        if (objects_.contains(name.lexeme())) {
            objects_[name.lexeme()] = std::move(object);
            return;
        }
        // chain
        if (enclosing_) {
            enclosing_->assign(name, std::move(object));
            return;
        }
        diagnostic_.error(name.line(), "Undefined variable '" + name.lexeme() + "'.");
        throw RuntimeError();
    }

    void print() {
        std::print("{}\n", objects_);
        if (enclosing_) {
            enclosing_->print();
        }
    }

private:
    std::unordered_map<std::string, Object> objects_;
    Diagnostic diagnostic_;
    EnvironmentPtr enclosing_ = nullptr;
};

} // cpplox