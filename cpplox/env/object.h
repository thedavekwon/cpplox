#pragma once

#include <format>
#include <string>
#include <variant>

#include <ast/statement.h>
#include <util/traits.h>

namespace cpplox {

class Interpreter;

using EnvironmentPtr = std::shared_ptr<class Environment>;
using FunctionPtr = std::shared_ptr<class Function>;
using NativeFunctionPtr = std::shared_ptr<class NativeFunction>;
using ClassPtr = std::shared_ptr<class Class>;
using InstancePtr = std::shared_ptr<class Instance>;

using Object = std::variant<std::nullptr_t, bool, double, std::string, FunctionPtr, NativeFunctionPtr, ClassPtr, InstancePtr>;

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

private:
    EnvironmentPtr closure_;
    const FunctionStatement& declaration_;
    bool isInit_;

    friend std::formatter<Function>;
    friend Interpreter;
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

    Object call(Interpreter* i, std::vector<Object> arguments);
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
            return it->second;
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

