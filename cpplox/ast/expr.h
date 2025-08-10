#pragma once

#include <format>
#include <memory>
#include <variant>

#include <scanner/token.h>

namespace cpplox {

using Expr = std::variant<struct AssignExpr, struct BinaryExpr, struct CallExpr, struct GetExpr, struct GroupingExpr, struct LiteralExpr, struct LogicalExpr, struct SetExpr, struct SuperExpr, struct ThisExpr, struct UnaryExpr, struct VarExpr>;

struct AssignExpr {
    Token name;
    std::unique_ptr<Expr> object;

    AssignExpr(Token n, Expr v);
};

struct BinaryExpr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(Expr l, Token o, Expr r);
};

struct CallExpr {
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<Expr> arguments;

    CallExpr(Expr c, Token p, std::vector<Expr> a);
};

struct GetExpr {
    std::unique_ptr<Expr> object;
    Token name;

    GetExpr(Expr o, Token n);
};

struct GroupingExpr {
    std::unique_ptr<Expr> expr;

    GroupingExpr(Expr e);
};

struct LiteralExpr {
    OptionalTokenLiteral object;
};

struct LogicalExpr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    LogicalExpr(Expr l, Token o, Expr r);
};

struct SetExpr {
    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;

    SetExpr(Expr o, Token n, Expr v);
};

struct SuperExpr {
    Token keyword;
    Token method;

    SuperExpr(Token k, Token m);
};

struct ThisExpr {
    Token keyword;

    ThisExpr(Token keyword);
};

struct UnaryExpr {
    Token op;
    std::unique_ptr<Expr> right;

    UnaryExpr(Token o, Expr r);
};

struct VarExpr {
    Token name;

    VarExpr(Token t);
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
        return std::visit([&](const auto& object) {
            return std::format_to(ctx.out(), "{}", object);
            }, e);
    }
};

template <>
struct std::formatter<cpplox::AssignExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::AssignExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "({} = {})", e.name.lexeme(), *e.object);
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
struct std::formatter<cpplox::CallExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::CallExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}({})", *e.callee, e.arguments);
    }
};

template <>
struct std::formatter<cpplox::GetExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::GetExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}.{}", *e.object, e.name.lexeme());
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
        if (!e.object.has_value()) {
            return std::format_to(ctx.out(), "");
        }
        return std::visit([&]<typename T>(const T & object) {
            if constexpr (std::is_same_v<T, std::string>) {
                return std::format_to(ctx.out(), "\"{}\"", object);
            }
            return std::format_to(ctx.out(), "{}", object);
        }, * e.object);
    }
};

template <>
struct std::formatter<cpplox::LogicalExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::LogicalExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{} {} {}", *e.left, (e.op.type() == cpplox::TokenType::AND) ? "and" : "or", *e.right);
    }
};

template <>
struct std::formatter<cpplox::SetExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::SetExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{}.{} = {}", *e.object, e.name.lexeme(), *e.value);
    }
};

template <>
struct std::formatter<cpplox::SuperExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::SuperExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "super.{}", e.method);
    }
};

template <>
struct std::formatter<cpplox::ThisExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::ThisExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "this");
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

template <>
struct std::formatter<cpplox::VarExpr> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::VarExpr& e, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{} {}", e.name.lexeme(), e.name.line());
    }
};