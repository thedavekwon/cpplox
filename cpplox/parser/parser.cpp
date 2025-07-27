#include <parser/parser.h>

namespace cpplox {

Expr Parser::expression() {
    return assignment();
}

Expr Parser::assignment() {
    Expr expr = logicalOr();

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

Expr Parser::logicalOr() {
    Expr expr = logicalAnd();
    while (match(TokenType::OR)) {
        Token op = previous();
        Expr right = logicalAnd();
        expr = LogicalExpr{ std::move(expr), std::move(op), std::move(right) };
    }
    return expr;
}

Expr Parser::logicalAnd() {
    Expr expr = equality();
    while (match(TokenType::AND)) {
        Token op = previous();
        Expr right = equality();
        expr = LogicalExpr{ std::move(expr), std::move(op), std::move(right) };
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
    if (match(TokenType::FOR)) {
        return forStatement();
    }
    if (match(TokenType::IF)) {
        return ifStatement();
    }
    if (match(TokenType::PRINT)) {
        return printStatement();
    }
    if (match(TokenType::WHILE)) {
        return whileStatement();
    }
    if (match(TokenType::LEFT_BRACE)) {
        return BlockStatement{ block() };
    }
    return expressionStatement();
}

Statement Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    Expr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    Statement thenBranch = statement();
    if (match(TokenType::ELSE)) {
        Statement elseBranch = statement();
        return IfStatement{ std::move(condition), std::move(thenBranch), std::move(elseBranch) };
    }
    return IfStatement{ std::move(condition), std::move(thenBranch) };
}

Statement Parser::printStatement() {
    Expr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return PrintStatement{ std::move(expr) };
}

Statement Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    // Initialzier
    std::optional<Statement> initializer;
    if (match(TokenType::SEMICOLON)) {
        // No initializer
    } else if (match(TokenType::VAR)) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    // Condition
    std::optional<Expr> condition;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    // Increment
    std::optional<Expr> increment;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    // Syntatic Sugar
    Statement body = statement();

    static_assert(std::is_nothrow_move_constructible_v<Statement>);

    if (increment.has_value()) {
        body = BlockStatement(std::move(body), ExprStatement{ std::move(*increment) });
    }

    if (!condition.has_value()) {
        condition = LiteralExpr{ "true" };
    }
    body = WhileStatement{ std::move(*condition), std::move(body) };

    if (initializer.has_value()) {
        body = BlockStatement(std::move(*initializer), std::move(body));
    }

    return body;
}

Statement Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after ')");
    Expr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    Statement body = statement();
    return WhileStatement{ std::move(condition), std::move(body) };
}

Statement Parser::expressionStatement() {
    Expr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return ExprStatement{ std::move(expr) };
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