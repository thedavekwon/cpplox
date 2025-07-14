#include <scanner/scanner.h>

#include <catch2/catch_test_macros.hpp>

using namespace cpplox;

namespace {

Token t(TokenType type, std::string lexeme) {
    return Token(type, std::move(lexeme), std::nullopt, 0);
}

}

// Add Exhaustive Token
TEST_CASE("ScannerSimple") {
    Diagnostic d;
    std::string s = "1 + 1";

    Scanner scanner(s, d);
    auto tokens = scanner.scanTokens();

    std::vector<Token> gt = { t(TokenType::NUMBER, "1"), t(TokenType::PLUS, "+"), t(TokenType::NUMBER, "1"), t(TokenType::EOFF, "") };
    REQUIRE(tokens.size() == gt.size());
    for (size_t i = 0; i < tokens.size(); i++) {
        REQUIRE(tokens[i].type() == gt[i].type());
        REQUIRE(tokens[i].lexeme() == gt[i].lexeme());
    }
}
