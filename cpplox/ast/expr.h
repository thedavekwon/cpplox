#pragma once

#include <format>
#include <memory>
#include <variant>

#include <scanner/token.h>

namespace cpplox {

class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;

using Expr = std::variant<BinaryExpr, GroupingExpr, LiteralExpr, UnaryExpr>;

struct BinaryExpr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(Expr l, Token o, Expr r);
};

struct GroupingExpr {
    std::unique_ptr<Expr> expr;

    GroupingExpr(Expr e);
};

struct LiteralExpr {
    OptionalTokenLiteral value;
};

struct UnaryExpr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token o, Expr r);
};

} // cpplox

template <>
struct std::formatter<cpplox::Expr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::Expr& e, FormatContext& ctx) const {
        return std::visit([&](const auto& value) {
            return std::format_to(ctx.out(), "{}", value);
            }, e);
    }
};

template <>
struct std::formatter<cpplox::BinaryExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::BinaryExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "({} {} {})", e.op.lexeme(), *e.left, *e.right);
    }
};

template <>
struct std::formatter<cpplox::GroupingExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::GroupingExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "(group {})", *e.expr);
    }
};

template <>
struct std::formatter<cpplox::LiteralExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::LiteralExpr& e, FormatContext& ctx) const {
        if (!e.value.has_value()) {
            return std::format_to(ctx.out(), "");
        }
        return std::visit([&]<typename T>(const T & value) {
            if constexpr (std::is_same_v<T, std::string>) {
                return std::format_to(ctx.out(), "\"{}\"", value);
            }
            return std::format_to(ctx.out(), "{}", value);
        }, * e.value);
    }
};

template <>
struct std::formatter<cpplox::UnaryExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::UnaryExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "({} {})", e.op.lexeme(), *e.right);
    }
};