#pragma once

#include <filesystem>
#include <string>

#include <diagnostic/diagnostic.h>

namespace cpplox {

class InterpreterDriver {
public:
    void runExpr(const std::string& program);
    void run(const std::string& program);
    void runScript(const std::filesystem::path& path);
    void runPrompt();
private:
    Diagnostic diagnostic_;
};

} // cpplox