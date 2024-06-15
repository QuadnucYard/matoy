#pragma once

#include "fwd.hpp"
#include "matoy/diag.hpp"
#include "matoy/foundations/approx.hpp"
#include "matoy/utils/match.hpp"
#include <compare>
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

inline auto equal(const Value& lhs, const Value& rhs) -> bool {
    return std::visit<bool>(utils::overloaded{                                              //
                                              []<class T>(T&& a, T&& b) { return a == b; }, //
                                              [](auto&&, auto&&) { return false; }},        //
                            lhs, rhs);
}

inline auto compare(const Value& lhs, const Value& rhs) -> diag::HintedResult<std::partial_ordering> {
    return std::visit<diag::HintedResult<std::partial_ordering>>(
        utils::overloaded{
            [](auto&& a, std::three_way_comparable_with<std::decay_t<decltype(a)>> auto&& b) { return a <=> b; },
            [](auto&& a, auto&& b) { return diag::hint_error(std::format("cannot compare {} and {}", a, b)); }},
        lhs, rhs);
}

inline auto eq(Value lhs, Value rhs) -> ValueResult {
    return equal(lhs, rhs);
}

inline auto neq(Value lhs, Value rhs) -> ValueResult {
    return !equal(lhs, rhs);
}

inline auto lt(Value lhs, Value rhs) -> ValueResult {
    return compare(lhs, rhs).transform([](auto ord) { return ord < 0; });
}

inline auto leq(Value lhs, Value rhs) -> ValueResult {
    return compare(lhs, rhs).transform([](auto ord) { return ord <= 0; });
}

inline auto gt(Value lhs, Value rhs) -> ValueResult {
    return compare(lhs, rhs).transform([](auto ord) { return ord > 0; });
}

inline auto geq(Value lhs, Value rhs) -> ValueResult {
    return compare(lhs, rhs).transform([](auto ord) { return ord >= 0; });
}

inline auto aeq(Value lhs, Value rhs) -> ValueResult {
    return std::visit<ValueResult>(
        utils::overloaded{
            []<class T>(T&& a, T&& b) { return foundations::approx(a, b); },
            [](auto&& a, auto&& b) { return diag::hint_error(std::format("cannot compare {} and {}", a, b)); }},
        lhs, rhs);
}

} // namespace matoy::eval
