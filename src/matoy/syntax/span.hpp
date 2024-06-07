#pragma once

#include <cstddef>

namespace matoy::syntax {

struct Span {
    std::size_t start{};
    std::size_t end{};
};

template <typename T>
struct Spanned {
    T v{};
    Span span{};
};

} // namespace matoy::syntax
