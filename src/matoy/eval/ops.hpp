#pragma once

#include "fwd.hpp"
#include "matoy/foundations/matrix.hpp"

namespace matoy::eval {

inline auto pos(Value rhs) -> ValueResult {
    return rhs.visit([](auto& v) -> ValueResult { return v; });
}

inline auto neg(Value rhs) -> ValueResult {
    return rhs.visit([](auto& v) -> ValueResult { return -v; });
}

inline auto add(Value lhs, Value rhs) -> ValueResult {
    // using foundations::Matrix;
    // using utils::overloaded;

    // auto ret = lhs.visit(overloaded{
    //     [&rhs](int64_t a) {
    //         return rhs.visit(overloaded{
    //             [a](int64_t b) -> Value { return a + b; },
    //             [a](double b) -> Value { return static_cast<double>(a) + b; },
    //             [a](const Matrix& b) -> Value { return a + b; },
    //         });
    //     },
    //     [&rhs](double a) {
    //         return rhs.visit(overloaded{
    //             [a](int64_t b) -> Value { return a + static_cast<double>(b); },
    //             [a](double b) -> Value { return a + b; },
    //             [a](const Matrix& b) -> Value { return a + b; },
    //         });
    //     },
    //     [&rhs](const Matrix& a) {
    //         return rhs.visit(overloaded{
    //             [a](int64_t b) -> Value { return a + b; },
    //             [a](double b) -> Value { return a + b; },
    //             [a](const Matrix& b) -> Value { return a + b; },
    //         });
    //     },
    // });
    // return ret;
    return lhs.visit([&rhs](auto& a) { return rhs.visit([&a](auto& b) -> ValueResult { return a + b; }); });
}

inline auto sub(Value lhs, Value rhs) -> ValueResult {
    return lhs.visit([&rhs](auto& a) { return rhs.visit([&a](auto& b) -> ValueResult { return a - b; }); });
}

inline auto mul(Value lhs, Value rhs) -> ValueResult {
    return lhs.visit([&rhs](auto& a) { return rhs.visit([&a](auto& b) -> ValueResult { return a * b; }); });
}

inline auto div(Value lhs, Value rhs) -> ValueResult {
    using foundations::Matrix;
    return lhs.visit([&rhs](auto& a) {
        return rhs.visit([&a](auto& b) -> ValueResult {
            using Tb = std::decay_t<decltype(b)>;
            if constexpr (std::is_same_v<Tb, Matrix>) {
                return {};
            } else {
                return a / b;
            }
        });
    });
}

} // namespace matoy::eval