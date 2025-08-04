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

    Object& getAt(size_t distance, const Token& name) {
        auto env = ancestor(distance);
        auto& curObject = env->objects_;
        if (auto it = curObject.find(name.lexeme()); it != curObject.end()) {
            return it->second;
        }
        diagnostic_.error(name.line(), "Undefined variable '" + name.lexeme() + "'.");
        throw RuntimeError();
    }

    Object& getAt(size_t distance, const std::string& name) {
        auto env = ancestor(distance);
        auto& curObject = env->objects_;
        if (auto it = curObject.find(name); it != curObject.end()) {
            return it->second;
        }
        diagnostic_.error(0, "Undefined variable '" + name + "'.");
        throw RuntimeError();
    }

    EnvironmentPtr ancestor(size_t distance) {
        EnvironmentPtr env = shared_from_this();
        for (size_t i = 0; i < distance; i++) {
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

    void assignAt(size_t distance, const Token& name, Object object) {
        ancestor(distance)->objects_[name.lexeme()] = std::move(object);
    }

    void print() {
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