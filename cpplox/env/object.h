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

using Object = std::variant<std::nullptr_t, bool, double, std::string, FunctionPtr, NativeFunctionPtr>;

template <typename T, typename = void>
constexpr bool is_callable_v = false;
template <typename T>
constexpr bool is_callable_v<T, std::void_t<decltype(std::declval<T>()->call(
    std::declval<Interpreter*>(),
    std::declval<std::vector<Object>>()
))>> = true;

class Function {
public:
    Function(EnvironmentPtr closure, const FunctionStatement& declaration) : closure_(std::move(closure)), declaration_(declaration) {}
    size_t arity() const { return declaration_.params.size(); }

private:
    EnvironmentPtr closure_;
    const FunctionStatement& declaration_;

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
