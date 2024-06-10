#pragma once

#include "fwd.hpp"
#include "matoy/diag.hpp"
#include "matoy/utils/match.hpp"
#include <type_traits>
#include <variant>

namespace matoy::eval {

inline auto pos(Value rhs) -> ValueResult {
    return std::move(rhs).visit<ValueResult>([](auto&& v) {
        if constexpr (requires { +v; }) {
            return +v;
        } else {
            return diag::hint_error(std::format("cannot apply unary '+' to {}", v));
        }
    });
}

inline auto neg(Value rhs) -> ValueResult {
    return std::move(rhs).visit<ValueResult>([](auto&& v) {
        if constexpr (requires { +v; }) {
            return -v;
        } else {
            return diag::hint_error(std::format("cannot apply unary '-' to {}", v));
        }
    });
}

inline auto not_(Value rhs) -> ValueResult {
    return std::move(rhs).visit<ValueResult>(
        utils::overloaded{[](bool&& v) { return !v; },
                          [](auto&& v) { return diag::hint_error(std::format("cannot apply 'not' to {}", v)); }});
}

inline auto add(Value lhs, Value rhs) -> ValueResult {
    return std::visit<ValueResult>(
        [](auto&& a, auto&& b) {
            if constexpr (requires { a + b; }) {
                return a + b;
            } else {
                return diag::hint_error(std::format("cannot add {} and {}", a, b));
            }
        },
        std::move(lhs), std::move(rhs));
}

inline auto sub(Value lhs, Value rhs) -> ValueResult {
    return std::visit<ValueResult>(
        [](auto&& a, auto&& b) {
            if constexpr (requires { a - b; }) {
                return a - b;
            } else {
                return diag::hint_error(std::format("cannot subtract {1} from {0}", a, b));
            }
        },
        std::move(lhs), std::move(rhs));
}

inline auto mul(Value lhs, Value rhs) -> ValueResult {
    return std::visit<ValueResult>(
        [](auto&& a, auto&& b) {
            if constexpr (requires { a* b; }) {
                return a * b;
            } else {
                return diag::hint_error(std::format("cannot multiply {} with {}", a, b));
            }
        },
        std::move(lhs), std::move(rhs));
}

inline auto div(Value lhs, Value rhs) -> ValueResult {
    return std::visit<ValueResult>(
        [](auto&& a, auto&& b) {
            if constexpr (requires { a / b; }) {
                return a / b;
            } else {
                return diag::hint_error(std::format("cannot divide {} by {}", a, b));
            }
        },
        std::move(lhs), std::move(rhs));
}

inline auto and_(Value lhs, Value rhs) -> ValueResult {
    return std::visit<ValueResult>(
        utils::overloaded{
            [](bool&& a, bool&& b) { return a && b; },
            [](auto&& a, auto&& b) { return diag::hint_error(std::format("cannot apply 'and' to {} and {}", a, b)); }},
        std::move(lhs), std::move(rhs));
}

inline auto or_(Value lhs, Value rhs) -> ValueResult {
    return std::visit<ValueResult>(
        utils::overloaded{
            [](bool&& a, bool&& b) { return a || b; },
            [](auto&& a, auto&& b) { return diag::hint_error(std::format("cannot apply 'and' to {} and {}", a, b)); }},
        std::move(lhs), std::move(rhs));
}

} // namespace matoy::eval
