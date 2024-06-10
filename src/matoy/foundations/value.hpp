#pragma once

#include "matoy/foundations/matrix.hpp"
#include <format>
#include <variant>

namespace matoy::foundations {

namespace values {

struct None {};

using none_t = None;
using int_t = int64_t;
using float_t = double;
using bool_t = bool;

} // namespace values

using Value = std::variant<values::none_t, values::int_t, values::float_t, values::bool_t, Matrix>;

} // namespace matoy::foundations

namespace matoy {
using foundations::Value;
namespace values = foundations::values;
} // namespace matoy

template <>
struct std::formatter<matoy::values::None> : std::formatter<char> {
    auto format(const matoy::values::None&, format_context& ctx) const {
        return std::format_to(ctx.out(), "none");
    }
};

template <>
struct std::formatter<matoy::Value> : std::formatter<char> {
    auto format(const matoy::Value& value, format_context& ctx) const {
        return value.visit([&ctx](auto& v) { return std::format_to(ctx.out(), "{}", v); });
    }
};
