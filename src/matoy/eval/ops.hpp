#pragma once

#include "fwd.hpp"
#include "matoy/foundations/matrix.hpp"
#include <variant>

namespace matoy::eval {

inline auto pos(Value rhs) -> ValueResult {
    return rhs.visit([](auto& v) -> ValueResult { return v; });
}

inline auto neg(Value rhs) -> ValueResult {
    return rhs.visit([](auto& v) -> ValueResult { return -v; });
}

inline auto add(Value lhs, Value rhs) -> ValueResult {
    return std::visit([](auto& a, auto& b) -> ValueResult { return a + b; }, lhs, rhs);
}

inline auto sub(Value lhs, Value rhs) -> ValueResult {
    return std::visit([](auto& a, auto& b) -> ValueResult { return a - b; }, lhs, rhs);
}

inline auto mul(Value lhs, Value rhs) -> ValueResult {
    return std::visit([](auto& a, auto& b) -> ValueResult { return a * b; }, lhs, rhs);
}

inline auto div(Value lhs, Value rhs) -> ValueResult {
    return std::visit(
        [](auto& a, auto& b) -> ValueResult {
            using Tb = std::decay_t<decltype(b)>;
            if constexpr (std::is_same_v<Tb, Matrix>) {
                return {};
            } else {
                return a / b;
            }
        },
        lhs, rhs);
}

} // namespace matoy::eval
