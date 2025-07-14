#include <parser/parser.h>

namespace cpplox {

Expr Parser::expression() {
    return equality();
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

    if (match(TokenType::LEFT_PAREN)) {
        Expr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return GroupingExpr{ std::move(expr) };
    }

    error(peek(), "Expect expression.");
    throw ParserError();
}


} // cpplox