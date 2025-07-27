#pragma once

#include <variant>

#include <ast/expr.h>

namespace cpplox {

using Statement = std::variant<struct BlockStatement, struct ExprStatement, struct IfStatement, struct PrintStatement, struct VarStatement, struct WhileStatement>;

struct BlockStatement {
    std::vector<Statement> statements;

    BlockStatement(std::vector<Statement> s);
    template <typename... S>
    BlockStatement(S&&... s) {
        static_assert(std::is_same_v<std::common_type_t<S...>, Statement>);
        (statements.push_back(std::forward<S>(s)), ...);
    }
};

struct ExprStatement {
    Expr expr;

    ExprStatement(Expr e);
};

struct IfStatement {
    Expr condition;
    // non-null
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;

    IfStatement(Expr c, Statement t);
    IfStatement(Expr c, Statement t, Statement e);
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

struct WhileStatement {
    Expr condition;
    std::unique_ptr<Statement> body;

    WhileStatement(Expr c, Statement b);
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
struct std::formatter<cpplox::IfStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::IfStatement& s, FormatContext& ctx) const {
        if (s.elseBranch) {
            return std::format_to(ctx.out(), "if ({}) {} else {}", s.condition, *s.thenBranch, *s.elseBranch);
        }
        return std::format_to(ctx.out(), "if ({}) {}", s.condition, *s.thenBranch);
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

template <>
struct std::formatter<cpplox::WhileStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::WhileStatement& s, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "while ({}) {};", s.condition, *s.body);
    }
};