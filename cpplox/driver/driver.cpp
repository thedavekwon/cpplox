#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>

#include <ast/expr.h>
#include <diagnostic/diagnostic.h>
#include <driver/driver.h>
#include <env/interpreter.h>
#include <env/resolver.h>
#include <parser/parser.h>
#include <scanner/scanner.h>

namespace cpplox {

InterpreterDriver::InterpreterDriver(std::ostream& out) : out_(out) {}

void InterpreterDriver::run(const std::string& program) {
    Scanner scanner(program, diagnostic_);
    auto tokens = scanner.scanTokens();
    if (diagnostic_.hadError()) {
        return;
    }

    Parser parser(tokens, diagnostic_);
    auto stmts = parser.parse();
    if (diagnostic_.hadError() || !stmts.has_value()) {
        return;
    }

    Interpreter interpreter(diagnostic_, out_);
    Resolver resolver(interpreter);
    resolver.resolve(*stmts);
    if (diagnostic_.hadError()) {
        return;
    }

    interpreter.interpret(*stmts);
    if (diagnostic_.hadError()) {
        return;
    }
}

void InterpreterDriver::runExpr(const std::string& program) {
    Scanner scanner(program, diagnostic_);
    auto tokens = scanner.scanTokens();
    if (diagnostic_.hadError()) {
        return;
    }
    std::print(out_, "tokens: {}\n", tokens);

    Parser parser(tokens, diagnostic_);
    auto expr = parser.parseExpr();
    if (diagnostic_.hadError() || !expr.has_value()) {
        return;
    }

    std::print(out_, "expression: {}\n", *expr);

    Interpreter interpreter(diagnostic_, out_);
    auto object = interpreter.interpretExpr(*expr);
    if (diagnostic_.hadError() || !expr.has_value()) {
        return;
    }
    std::print(out_, "object: {}\n", *object);
}

void InterpreterDriver::runScript(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        diagnostic_.error(0, "Could not open file.");
        return;
    }
    run({ std::istreambuf_iterator<char>(file), {} });
    if (diagnostic_.hadError()) {
        exit(65);
    }
}

void InterpreterDriver::runPrompt() {
    Interpreter interpreter(diagnostic_, out_);
    Resolver resolver(interpreter);
    resolver.beginScope();
    std::string line;
    while (true) {
        std::print(out_, "> ");
        if (!std::getline(std::cin, line)) break;

        Scanner scanner(line, diagnostic_);
        auto tokens = scanner.scanTokens();
        if (diagnostic_.hadError()) {
            return;
        }

        Parser parser(tokens, diagnostic_);
        auto stmts = parser.parse();
        if (diagnostic_.hadError() || !stmts.has_value()) {
            return;
        }

        resolver.resolve(*stmts, false);
        if (diagnostic_.hadError()) {
            return;
        }

        interpreter.interpret(*stmts);
        if (diagnostic_.hadError()) {
            return;
        }

        diagnostic_.reset();
    }
    resolver.endScope();
}

} // cpplox

