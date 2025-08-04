#pragma once

#include <variant>

#include <ast/expr.h>

namespace cpplox {

using Statement = std::variant<struct BlockStatement, struct ClassStatement, struct ExprStatement, struct FunctionStatement, struct IfStatement, struct PrintStatement, struct ReturnStatement, struct VarStatement, struct WhileStatement>;

struct BlockStatement {
    std::vector<Statement> statements;

    BlockStatement(std::vector<Statement> s);
    template <typename... S> requires std::is_same_v<std::common_type_t<S...>, Statement>
    BlockStatement(S&&... s) {
        (statements.push_back(std::forward<S>(s)), ...);
    }
};

struct ClassStatement {
    Token name;
    std::vector<FunctionStatement> methods;

    ClassStatement(Token n, std::vector<FunctionStatement> m);
};

struct ExprStatement {
    Expr expr;

    ExprStatement(Expr e);
};

struct FunctionStatement {
    Token name;
    std::vector<Token> params;
    std::unique_ptr<BlockStatement> body;

    FunctionStatement(Token n, std::vector<Token> p, BlockStatement b);
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

struct ReturnStatement {
    Token keyword;
    std::optional<Expr> value;

    ReturnStatement(Token keyword, std::optional<Expr> value);
};

struct VarStatement {
    Token name;
    std::optional<Expr> initializer;

    VarStatement(Token token, std::optional<Expr> init = std::nullopt);
};

struct WhileStatement {
    Expr condition;
    std::unique_ptr<BlockStatement> body;

    WhileStatement(Expr c, BlockStatement b);
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
        return std::visit([&](const auto& object) {
            return std::format_to(ctx.out(), "{}", object);
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
        return std::format_to(ctx.out(), "{{{}}}", s.statements);
    }
};

template <>
struct std::formatter<cpplox::ClassStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::ClassStatement& s, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "class {}", s.name.lexeme());
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
struct std::formatter<cpplox::FunctionStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::FunctionStatement& s, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "fun {}({}) {};", s.name.lexeme(), s.params, *s.body);
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
struct std::formatter<cpplox::ReturnStatement> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const cpplox::ReturnStatement& s, FormatContext& ctx) const {
        if (s.value.has_value()) {
            return std::format_to(ctx.out(), "return {};", *s.value);
        }
        return std::format_to(ctx.out(), "return nil;");
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