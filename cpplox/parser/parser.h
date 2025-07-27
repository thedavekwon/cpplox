#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <ast/expr.h>
#include <ast/statement.h>
#include <diagnostic/diagnostic.h>
#include <scanner/token.h>

namespace cpplox {

// Lox grammar with associativity and precedence
// 
// program     -> declaration* EOF;
//
// declaration -> varDecl | statement;
// 
// varDecl     -> "var" IDENTIFIER ("=" expression)?";"; 
//
// statement   -> exprStmt | forStmt | ifStmt | printStmt | whileStmt | block;
// exprStmt    -> expression ";";
// forStmt     -> "for" "(" (varDecl | exprStmt | ";") expression? ";" expression? ")" statement;
// ifStmt      -> "if" "(" expression ")" statement ("else" statement)?;
// printStmt   -> "print" expression ";";
// whileStmt   -> "while" "(" expression ")" statement;
// block       -> "{" declaration* "}";
//
// expression  -> assignment;
// assignment  -> IDENTIFIER "=" assignment | logic_or;
// logic_or    -> logic_and ("or" logic_and)*;
// logic_and   -> equality ("and" eqaulity)*;
// equality    -> comparison (("!=" | "==") comparison)*;
// comparison  -> term ((">"|">="|"<"|"<=") term)*;
// term        -> factor (("-"|"+") factor)*;
// factor      -> factor ("/" | "*") unary | unary; // LEFT associative
// factor      -> unary (("/" | "*") unary)*;       // RIGHT associative
// unary       -> ("!" | "-") unary | primary;
// primary     -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER;

// Top-down predictive parser
class Parser {
public:
    Parser(const std::vector<Token>& tokens, Diagnostic& diagnostic) : tokens_(tokens), diagnostic_(diagnostic) {}

    std::optional<Expr> parseExpr() {
        try {
            return expression();
        } catch (const ParserError&) {
            return std::nullopt;
        }
    }

    std::optional<std::vector<Statement>> parse() {
        std::vector<Statement> statements;
        while (!isAtEnd()) {
            if (auto decl = declaration()) {
                statements.push_back(std::move(*decl));
                continue;
            }
        }
        return statements;
    }

private:
    // Parsing expressions
    Expr expression();
    Expr assignment();
    Expr logicalOr();
    Expr logicalAnd();
    Expr equality();
    Expr comparison();
    Expr term();
    Expr factor();
    Expr unary();
    Expr primary();

    // Parsing statements
    std::optional<Statement> declaration();
    Statement varDeclaration();
    Statement statement();
    Statement ifStatement();
    Statement forStatement();
    Statement whileStatement();
    Statement printStatement();
    Statement expressionStatement();
    std::vector<Statement> block();

    // Helper functions
    template <typename... T>
    bool match(T... types) {
        static_assert(std::is_same_v<std::common_type_t<T...>, TokenType>);
        return ((check(types) ? (advance(), true) : false) || ...);
    }

    bool check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type() == type;
    }
    bool isAtEnd() {
        return peek().type() == TokenType::EOFF;
    }
    const Token& peek() const {
        return tokens_[current_];
    }
    const Token& previous() const {
        return tokens_[current_ - 1];
    }
    const Token& advance() {
        if (!isAtEnd()) current_++;
        return previous();
    }

    Token consume(TokenType type, std::string_view message) {
        if (check(type)) return advance();
        error(peek(), message);
        throw ParserError();
    }

    class ParserError : public std::runtime_error {
    public:
        ParserError() : std::runtime_error("Parser error") {}
    };

    void error(Token token, std::string_view message) {
        if (token.type() == TokenType::EOFF) {
            diagnostic_.report(token.line(), "at end", message);
        } else {
            diagnostic_.report(token.line(), "at '" + token.lexeme() + "'", message);
        }
    }

    // Discards token until finding a statement boundary.
    void synchronize() {
        advance();
        while (!isAtEnd()) {
            if (previous().type() == TokenType::SEMICOLON) return;

            switch (peek().type()) {
                case TokenType::CLASS:
                case TokenType::FOR:
                case TokenType::FUN:
                case TokenType::IF:
                case TokenType::PRINT:
                case TokenType::RETURN:
                case TokenType::VAR:
                case TokenType::WHILE:
                    return;
                default:
                    break;
            }
            advance();
        }
    }

private:
    int current_ = 0;
    const std::vector<Token>& tokens_;
    Diagnostic& diagnostic_;
};

} // cpplox