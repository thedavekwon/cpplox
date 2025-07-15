#pragma once

#include <variant>

#include <ast/expr.h>

namespace cpplox {

using Statement = std::variant<struct BlockStatement, struct ExprStatement, struct PrintStatement, struct VarStatement>;

struct BlockStatement {
    std::vector<Statement> statements;

    BlockStatement(std::vector<Statement> s);
};

struct ExprStatement {
    Expr expr;

    ExprStatement(Expr e);
};

struct PrintStatement {
    Expr expr;

    PrintStatement(Expr e);
};

struct VarStatement {
    Token name;
    std::optional<Expr> initializer;

    VarStatement(Token token, std::optional<Expr> init = std::nullopt);
};

} // cpplox

template <>
struct std::formatter<cpplox::Statement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::Statement& s, FormatContext& ctx) const {
        return std::visit([&](const auto& value) {
            return std::format_to(ctx.out(), "{}", value);
            }, s);
    }
};

template <>
struct std::formatter<cpplox::BlockStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::BlockStatement& s, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{{}};", s.statements);
    }
};

template <>
struct std::formatter<cpplox::ExprStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::ExprStatement& s, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "{};", s.expr);
    }
};

template <>
struct std::formatter<cpplox::PrintStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::PrintStatement& s, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "print {};", s.expr);
    }
};

template <>
struct std::formatter<cpplox::VarStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::VarStatement& s, FormatContext& ctx) const {
        if (s.initializer.has_value()) {
            return std::format_to(ctx.out(), "var {} = {};", s.name.lexeme(), *s.initializer);
        }
        return std::format_to(ctx.out(), "var {};", s.name.lexeme());
    }
};