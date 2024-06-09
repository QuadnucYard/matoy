#pragma once

#include <string>
#include <string_view>

namespace matoy::utils {

inline auto slice(std::string_view s, size_t start, size_t end) -> std::string_view {
    return s.substr(start, end - start);
}

inline auto slice(const std::string& s, size_t start, size_t end) -> std::string_view {
    return slice(std::string_view{s}, start, end);
}

} // namespace matoy::utils
