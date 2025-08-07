#pragma once

#include <filesystem>
#include <string>
#include <iostream>

#include <diagnostic/diagnostic.h>

namespace cpplox {

class InterpreterDriver {
public:
    explicit InterpreterDriver(std::ostream& out = std::cout);
    void runExpr(const std::string& program);
    void run(const std::string& program);
    void runScript(const std::filesystem::path& path);
    void runPrompt();
private:
    Diagnostic diagnostic_;
    std::ostream& out_;
};

} // cpplox