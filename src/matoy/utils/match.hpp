#pragma once

namespace matoy::utils {

// a `matches!` like checker
template <auto... args, class T>
inline auto is_in(const T& value) {
    return ((value == args) || ...);
}

// helper type for the visitor
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

} // namespace matoy::utils
