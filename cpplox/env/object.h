#pragma once

#include <format>
#include <string>
#include <variant>


#include <env/env.h>
#include <env/fwd.h>
#include <ast/statement.h>
#include <util/traits.h>

namespace cpplox {

template <typename T, typename = void>
constexpr bool is_callable_v = false;
template <typename T>
constexpr bool is_callable_v<T, std::void_t<decltype(std::declval<T>()->call(
    std::declval<Interpreter*>(),
    std::declval<std::vector<Object>>()
))>> = true;

class Function {
public:
    Function(EnvironmentPtr closure, const FunctionStatement& declaration, bool isInit) : closure_(std::move(closure)), declaration_(declaration), isInit_(isInit) {}
    size_t arity() const { return declaration_.params.size(); }
    template <typename T> requires std::is_same_v<T, Interpreter>
    Object call(T* i, std::vector<Object> arguments) {
        EnvironmentPtr env = std::make_shared<Environment>(closure_);
        for (size_t i = 0; i < arity(); i++) {
            env->define(declaration_.params[i].lexeme(), std::move(arguments[i]));
        }
        auto ret = i->operator()(*declaration_.body, env);
        if (isInit_) {
            return closure_->getAt(0, "this");
        }
        if (ret.has_value()) {
            return ret.value();
        }
        return nullptr;
    }
    FunctionPtr bind(InstancePtr instance);

private:
    EnvironmentPtr closure_;
    const FunctionStatement& declaration_;
    bool isInit_;

    friend std::formatter<Function>;
    friend Interpreter;
    friend Class;
};

class NativeFunction {
public:
    NativeFunction(std::string name, size_t arity, std::function<Object(Interpreter*, std::vector<Object>)> call) : name_(std::move(name)), arity_(arity), call_(std::move(call)) {}
    Object call(Interpreter* i, std::vector<Object> arguments);
    size_t arity() const { return arity_; }

private:
    std::string name_;
    size_t arity_;
    std::function<Object(Interpreter*, std::vector<Object>)> call_;
    friend std::formatter<NativeFunction>;
};

class Class : public std::enable_shared_from_this<Class> {
public:
    Class(std::string name, std::unordered_map<std::string, FunctionPtr> methods) : name_(std::move(name)), methods_(std::move(methods)) {}

    template <typename T> requires std::is_same_v<T, Interpreter>
    Object call(T* i, std::vector<Object> arguments) {
        auto instance = std::make_shared<Instance>(shared_from_this());
        auto init = findMethod("init");
        if (init) {
            init->bind(instance)->call(i, std::move(arguments));
        }
        return instance;
    }
    size_t arity() const;
    FunctionPtr findMethod(const std::string& name) {
        if (auto it = methods_.find(name); it != methods_.end()) {
            return it->second;
        }
        return nullptr;
    }

private:
    std::string name_;
    std::unordered_map<std::string, FunctionPtr> methods_;
    friend Instance;
    friend Interpreter;
    friend std::formatter<Class>;
};

class Instance : public std::enable_shared_from_this<Instance> {
public:
    Instance(ClassPtr c) : class_(std::move(c)) {}

    std::optional<Object> get(const Token& name) {
        if (auto it = fields_.find(name.lexeme()); it != fields_.end()) {
            return it->second;
        }

        if (auto it = class_->methods_.find(name.lexeme()); it != class_->methods_.end()) {
            return it->second->bind(shared_from_this());
        }

        return std::nullopt;
    }

    void set(const Token& name, Object value) {
        fields_[name.lexeme()] = std::move(value);
    }

private:
    ClassPtr class_;
    std::unordered_map<std::string, Object> fields_;
    friend Interpreter;
    friend std::formatter<Instance>;
};

} // cpplox

template <>
struct std::formatter<cpplox::Object> : std::formatter<std::string> {
    template<typename FormatContext>
    auto format(const cpplox::Object& object, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}", std::visit([]<typename T>(const T & v) -> std::string {
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return "nil";
            } else if constexpr (std::is_same_v<T, bool>) {
                return v ? "true" : "false";
            } else if constexpr (std::is_same_v<T, double>) {
                return std::format("{}", v);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return std::format("\"{}\"", v);
            } else if constexpr (cpplox::is_derefable_v<T>) {
                return std::format("{}", *v);
            } else {
                return std::format("{}", v);
            }
        }, object));
    }
};

template <>
struct std::formatter<cpplox::Function> : std::formatter<std::string> {
    template<typename FormatContext>
    auto format(const cpplox::Function& object, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "<fn {}>", object.declaration_.name);
    }
};

template <>
struct std::formatter<cpplox::NativeFunction> : std::formatter<std::string> {
    template<typename FormatContext>
    auto format(const cpplox::NativeFunction& object, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "<native fn {}>", object.name_);
    }
};

template <>
struct std::formatter<cpplox::Class> : std::formatter<std::string> {
    template<typename FormatContext>
    auto format(const cpplox::Class& object, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "<class {}>", object.name_);
    }
};

template <>
struct std::formatter<cpplox::Instance> : std::formatter<std::string> {
    template<typename FormatContext>
    auto format(const cpplox::Instance& object, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "<instance of {}>", *object.class_);
    }
};

