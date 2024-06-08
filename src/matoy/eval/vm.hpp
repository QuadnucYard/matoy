#pragma once

#include "scope.hpp"

namespace matoy::eval {

struct Vm {
    Scopes scopes;

    Vm() = default;
    Vm(const Vm&) = delete;
    Vm& operator=(const Vm&) = delete;

    auto define(auto&& name, auto&& value) -> void {
        scopes.top().define(std::move(name), std::move(value));
    }
};

} // namespace matoy::eval
