#pragma once

#include <cctype>
#include <charconv>
#include <cstdlib>
#include <optional>
#include <string_view>
#include <system_error>

namespace matoy::syntax {

template <typename T, typename... Args>
inline auto is_in(const T& value, const Args&&... args) {
    return ((value == args) || ...);
}

inline auto is_space(char c) -> bool { return std::isspace(c); }

inline auto is_alpha(char c) -> bool { return std::isalpha(c); }

inline auto is_digit(char c) -> bool { return std::isdigit(c); }

inline auto is_alnum(char c) -> bool { return std::isalnum(c); }

template <std::integral T>
inline auto parse_int(std::string_view s, int base = 10) -> std::optional<T> {
    T value{};
#if __cpp_lib_to_chars >= 202306L
    if (std::from_chars(s.data(), s.data() + s.size(), value, base))
#else
    if (std::from_chars(s.data(), s.data() + s.size(), value, base).ec == std::errc{})
#endif
        return value;
    else
        return std::nullopt;
};

inline auto parse_double(std::string_view s) -> std::optional<double> {
    char* end{};
    double value = std::strtod(s.data(), &end);
    if (end != s.data())
        return value;
    else
        return std::nullopt;
};

} // namespace matoy::syntax
