#pragma once

#include "matoy/foundations/matrix.hpp"
#include "matoy/utils/match.hpp"
#include <format>
#include <variant>

namespace matoy::foundations {

using Value = std::variant<int64_t, double, Matrix>;

} // namespace matoy::foundations

template <>
struct std::formatter<matoy::foundations::Value> : std::formatter<char> {
    auto format(const matoy::foundations::Value& value, format_context& ctx) const {
        std::format_to(ctx.out(), "Value(");
        value.visit(matoy::utils::overloaded{
            [&ctx](const int64_t& v) { return std::format_to(ctx.out(), "{}", v); },
            [&ctx](const double& v) { return std::format_to(ctx.out(), "{}", v); },
            [&ctx](const matoy::foundations::Matrix&) { return std::format_to(ctx.out(), "[Matrix]"); },
        });
        return std::format_to(ctx.out(), ")");
    }
};
