#pragma once

#include <cstddef>
#include <format>

namespace matoy::syntax {

struct Span {
    std::size_t start{};
    std::size_t end{};

    Span operator|(const Span& other) const {
        return {std::min(start, other.start), std::max(end, other.end)};
    }
};

template <typename T>
struct Spanned {
    T v{};
    Span span{};
};

} // namespace matoy::syntax

template <>
struct std::formatter<matoy::syntax::Span> : std::formatter<char> {
    auto format(const matoy::syntax::Span& span, format_context& ctx) const {
        return std::format_to(ctx.out(), "{}:{}", span.start, span.end);
    }
};
