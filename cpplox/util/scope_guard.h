#pragma once

#include <functional>

namespace cpplox {

class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> guard) : guard_(std::move(guard)) {}

    ~ScopeGuard() {
        guard_();
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;

private:
    std::function<void()> guard_;
};

} // cpplox