#pragma once

#include "matoy/foundations/matrix.hpp"
#include <format>
#include <variant>

namespace matoy::foundations {

using Value = std::variant<int64_t, double, Matrix>;

} // namespace matoy::foundations

namespace matoy {
using foundations::Value;
}

template <>
struct std::formatter<matoy::Value> : std::formatter<char> {
    auto format(const matoy::Value& value, format_context& ctx) const {
        // std::format_to(ctx.out(), "Value(");
        return value.visit([&ctx](auto& v) { return std::format_to(ctx.out(), "{}", v); });
        // return std::format_to(ctx.out(), ")");
    }
};
