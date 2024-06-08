#pragma once

#include <functional>
#include <string_view>

namespace matoy::utils {

// allow comparing keys of different types
struct transparent_string_hash {
    using is_transparent = void;

    std::size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
};

} // namespace matoy::utils
