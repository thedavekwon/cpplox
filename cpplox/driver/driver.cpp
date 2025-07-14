#include <fstream>
#include <iostream>
#include <iterator>
#include <filesystem>

#include <ast/expr.h>
#include <diagnostic/diagnostic.h>
#include <driver/driver.h>
#include <env/interpreter.h>
#include <parser/parser.h>
#include <scanner/scanner.h>

namespace cpplox {

void InterpreterDriver::run(const std::string& program) {
    Scanner scanner(program, diagnostic_);
    auto tokens = scanner.scanTokens();
    if (diagnostic_.hadError()) {
        return;
    }
    std::print("tokens: {}\n", tokens);

    Parser parser(tokens, diagnostic_);
    auto expr = parser.parse();
    if (diagnostic_.hadError() || !expr.has_value()) {
        return;
    }

    std::print("expression: {}\n", *expr);

    Interpreter interpreter(diagnostic_);
    auto value = interpreter.interpret(*expr);
    if (diagnostic_.hadError() || !expr.has_value()) {
        return;
    }
    std::print("value: {}\n", *value);
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
    std::string line;
    while (true) {
        std::print("> ");
        if (!std::getline(std::cin, line)) break;
        run(line);
        diagnostic_.reset();
    }
}

} // cpplox
