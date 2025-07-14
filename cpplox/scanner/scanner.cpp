#include <optional>
#include <unordered_map>
#include <vector>

#include <scanner/scanner.h>


namespace {

bool isAlphaNumeric(char c) {
    return isalpha(c) || c == '_' || isdigit(c);
}

}

namespace cpplox {

Token Scanner::getToken(TokenType type) {
    return getToken(type, std::nullopt);
}
Token Scanner::getToken(TokenType type, OptionalTokenLiteral literal) {
    return Token(type, source_.substr(start_, current_ - start_), std::move(literal), line_);
}

std::optional<Token> Scanner::scanToken() {
    char c = advance();
    switch (c) {
        case '(':
            return getToken(TokenType::LEFT_PAREN);
        case ')':
            return getToken(TokenType::RIGHT_PAREN);
        case '{':
            return getToken(TokenType::LEFT_BRACE);
        case '}':
            return getToken(TokenType::RIGHT_BRACE);
        case ',':
            return getToken(TokenType::COMMA);
        case '.':
            return getToken(TokenType::DOT);
        case '-':
            return getToken(TokenType::MINUS);
        case '+':
            return getToken(TokenType::PLUS);
        case ';':
            return getToken(TokenType::SEMICOLON);
        case '*':
            return getToken(TokenType::STAR);
        case '!':
            return getToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=':
            return getToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<':
            return getToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return getToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '/': {
            if (match('/')) {
                // Scan a comment until EOL and discard it.
                while (peek() != '\n' && !isAtEnd()) advance();
            } else {
                return getToken(TokenType::SLASH);
            }
        }
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            return std::nullopt;
        case '\n':
            line_++;
            return std::nullopt;
        case '"':
            return getStringLiteralToken();
        default: {
            if (isdigit(c)) {
                return getNumberLiteralToken();
            } else if (isalpha(c) || c == '_') {
                return getKeywordOrIdentifierToken();
            }
            diagnostic_.error(line_, "Unexpected character.");
            return getToken(TokenType::ERROR);
        }
    };
    std::unreachable();
}

std::vector<Token> Scanner::scanTokens() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        start_ = current_;
        auto t = scanToken();
        if (t) tokens.push_back(std::move(*t));
    }
    tokens.emplace_back(TokenType::EOFF, "", std::nullopt, line_);
    return tokens;
}

Token Scanner::getStringLiteralToken() {
    while (peek() != '"' && !isAtEnd()) {
        // multi-line string
        if (peek() == '\n') line_++;
        advance();
    }

    if (isAtEnd()) {
        diagnostic_.error(line_, "Unterminated string.");
        return getToken(TokenType::ERROR);
    }

    // We don't handle escape sequences. 

    // The closing ".
    advance();
    return getToken(TokenType::STRING, source_.substr(start_ + 1, current_ - start_ - 2));
}

Token Scanner::getNumberLiteralToken() {
    // Scan leading digits
    while (isdigit(peek())) advance();

    if (peek() == '.' && isdigit(peekNext())) {
        advance();
        // Scan trailing digits
        while (isdigit(peek())) advance();
    }
    return getToken(TokenType::NUMBER, std::stod(source_.substr(start_, current_ - start_)));
}

Token Scanner::getKeywordOrIdentifierToken() {
    while (isAlphaNumeric(peek())) advance();

    std::string identifier = source_.substr(start_, current_ - start_);
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"and", TokenType::AND},
        {"class", TokenType::CLASS},
        {"else", TokenType::ELSE},
        {"false", TokenType::FALSE},
        {"for", TokenType::FOR},
        {"fun", TokenType::FUN},
        {"if", TokenType::IF},
        {"nil", TokenType::NIL},
        {"or", TokenType::OR},
        {"print", TokenType::PRINT},
        {"return", TokenType::RETURN},
        {"super", TokenType::SUPER},
        {"this", TokenType::THIS},
        {"true", TokenType::TRUE},
        {"var", TokenType::VAR},
        {"while", TokenType::WHILE},
    };
    auto it = keywords.find(identifier);
    if (it != keywords.end()) {
        return getToken(it->second);
    }
    return getToken(TokenType::IDENTIFIER);
}

} // cpplox
