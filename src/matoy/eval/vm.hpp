#pragma once

#include "scope.hpp"

namespace matoy::eval {

struct Vm {
    Scopes scopes;

    Vm() = default;
    Vm(const Vm&) = delete;
    Vm& operator=(const Vm&) = delete;
};

} // namespace matoy::eval
