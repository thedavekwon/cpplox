#include <diagnostic/diagnostic.h>
#include <iostream>
#include <print>
#include <string_view>

namespace cpplox {

void Diagnostic::report(int line, std::string_view where, std::string_view message) {
    std::println(stderr, "[line {}] Error {}: {}", line, where, message);
    hadError_ = true;
}

void Diagnostic::error(int line, std::string_view message) {
    report(line, "", message);
}

bool Diagnostic::hadError() {
    return hadError_;
}

void Diagnostic::reset() {
    hadError_ = false;
}

} // cpplox
