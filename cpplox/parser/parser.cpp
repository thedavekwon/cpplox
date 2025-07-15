#include <parser/parser.h>

namespace cpplox {

Expr Parser::expression() {
    return assignment();
}

Expr Parser::assignment() {
    Expr expr = equality();

    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        Expr value = assignment();
        if (auto* var = std::get_if<VarExpr>(&expr)) {
            return AssignExpr{ std::move(var->name), std::move(value) };
        }
        error(equals, "Invalid assignment target.");
    }
    return expr;
}

Expr Parser::equality() {
    Expr expr = comparison();
    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
        Token op = previous();
        Expr right = comparison();
        expr = BinaryExpr{ std::move(expr), std::move(op), std::move(right) };
    }
    return expr;
}

Expr Parser::comparison() {
    Expr expr = term();
    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
        Token op = previous();
        Expr right = term();
        expr = BinaryExpr{ std::move(expr), std::move(op), std::move(right) };
    }
    return expr;
}

Expr Parser::term() {
    Expr expr = factor();
    while (match(TokenType::MINUS, TokenType::PLUS)) {
        Token op = previous();
        Expr right = factor();
        expr = BinaryExpr{ std::move(expr), std::move(op), std::move(right) };
    }
    return expr;
}

Expr Parser::factor() {
    Expr expr = unary();
    while (match(TokenType::SLASH, TokenType::STAR)) {
        Token op = previous();
        Expr right = unary();
        expr = BinaryExpr{ std::move(expr), std::move(op), std::move(right) };
    }
    return expr;
}

Expr Parser::unary() {
    if (match(TokenType::BANG, TokenType::MINUS)) {
        Token op = previous();
        Expr right = unary();
        return UnaryExpr{ std::move(op), std::move(right) };
    } else {
        return primary();
    }
}

Expr Parser::primary() {
    if (match(TokenType::FALSE)) return LiteralExpr{ "false" };
    if (match(TokenType::TRUE)) return LiteralExpr{ "true" };
    if (match(TokenType::NIL)) return LiteralExpr{ "nil" };

    if (match(TokenType::NUMBER, TokenType::STRING)) {
        return LiteralExpr{ previous().literal() };
    }

    if (match(TokenType::IDENTIFIER)) {
        return VarExpr(previous());
    }

    if (match(TokenType::LEFT_PAREN)) {
        Expr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return GroupingExpr{ std::move(expr) };
    }

    error(peek(), "Expect expression.");
    throw ParserError();
}

std::optional<Statement> Parser::declaration() {
    try {
        if (match(TokenType::VAR)) {
            return varDeclaration();
        }
        return statement();
    } catch (const ParserError&) {
        synchronize();
        return std::nullopt;
    }
}

Statement Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    std::optional<Expr> init;
    if (match(TokenType::EQUAL)) {
        init = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return VarStatement{ std::move(name), std::move(init) };
}

Statement Parser::statement() {
    if (match(TokenType::PRINT)) {
        return printStatement();
    }
    if (match(TokenType::LEFT_BRACE)) {
        return BlockStatement{ block() };
    }
    return expressionStatement();
}

Statement Parser::printStatement() {
    Expr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return { PrintStatement{std::move(expr)} };
}

Statement Parser::expressionStatement() {
    Expr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return { ExprStatement{std::move(expr)} };
}

std::vector<Statement> Parser::block() {
    std::vector<Statement> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (auto decl = declaration()) {
            statements.push_back(std::move(*decl));
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}


} // cpplox