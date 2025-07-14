#pragma once

#include <string_view>

namespace cpplox {

class Diagnostic {
public:
    void report(int line, std::string_view where, std::string_view message);
    void error(int line, std::string_view message);
    bool hadError();
    void reset();
    void print();

private:
    bool hadError_ = false;;
};

} // cpplox