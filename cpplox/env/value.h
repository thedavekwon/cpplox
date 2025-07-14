#pragma once

#include <format>
#include <string>
#include <variant>

namespace cpplox {

using Value = std::variant<std::nullptr_t, bool, double, std::string>;

} // cpplox

template <>
struct std::formatter<cpplox::Value> : std::formatter<std::string> {
    template<typename FormatContext>
    auto format(const cpplox::Value& value, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}", std::visit([]<typename T>(const T & v) -> std::string {
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return "nil";
            } else if constexpr (std::is_same_v<T, bool>) {
                return v ? "true" : "false";
            } else if constexpr (std::is_same_v<T, double>) {
                return std::format("{}", v);
            } else {
                return std::format("\"{}\"", v);
            }
        }, value));
    }
};