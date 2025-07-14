#pragma once

#include <string>
#include <vector>

#include <scanner/token.h>
#include <diagnostic/diagnostic.h>

namespace cpplox {

class Scanner {
public:
    Scanner(const std::string& source, Diagnostic& diagnostic) : source_(source), diagnostic_(diagnostic) {}
    std::vector<Token> scanTokens();
private:
    std::optional<Token> scanToken();
    Token getToken(TokenType type);
    Token getToken(TokenType type, OptionalTokenLiteral literal);
    char advance() {
        return source_[current_++];
    }
    // Advance if the expected character is found.
    bool match(char expected) {
        if (isAtEnd()) {
            return false;
        }
        if (source_[current_] != expected) return false;
        current_++;
        return true;
    }
    // lookahead
    char peek() {
        if (isAtEnd()) return '\0';
        return source_[current_];
    }
    char peekNext() {
        if (current_ + 1 >= source_.length()) return '\0';
        return source_[current_ + 1];
    }
    bool isAtEnd() const {
        return current_ >= source_.length();
    }

    // We don't support escape sequences.
    Token getStringLiteralToken();
    // A series of digits optionally followed by a '.' and one or more trailing digits
    Token getNumberLiteralToken();
    Token getKeywordOrIdentifierToken();

private:
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;
    const std::string& source_;
    Diagnostic& diagnostic_;
};

} // cpplox