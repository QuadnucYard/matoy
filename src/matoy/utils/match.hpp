#pragma once

namespace matoy::utils {

// a `matches!` like checker
template <typename T, typename... Args>
inline auto is_in(const T& value, const Args&&... args) {
    return ((value == args) || ...);
}

// helper type for the visitor
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

} // namespace matoy::utils
