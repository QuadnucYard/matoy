#pragma once

#include <cmath>
#include <concepts>
#include <limits>
#include <type_traits>

namespace matoy::foundations {

template <class T>
concept non_floating_point = !std::is_floating_point_v<T>;

// Test whether two float or double numbers are equal.
// ulp: units in the last place.
// template <class T>
// auto approx(const T& x, const T& y, int ulp = 2) -> bool;

// template <non_floating_point T>
// auto approx(const T& x, const T& y, int) -> bool {
//     return x == y;
// }

// template <std::floating_point T>
// auto approx(const T& x, const T& y, int ulp) -> bool {
//     // the machine epsilon has to be scaled to the magnitude of the values used
//     // and multiplied by the desired precision in ULPs (units in the last place)
//     return std::fabs(x - y) < std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
//            // unless the result is subnormal
//            || std::fabs(x - y) < std::numeric_limits<T>::min();
// }

template <class T>
auto approx(const T& x, const T& y, int ulp = 2) -> bool {
    if constexpr (std::floating_point<T>) {
        return std::fabs(x - y) < std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp ||
               std::fabs(x - y) < std::numeric_limits<T>::min();
    } else {
        return x == y;
    }
}

} // namespace matoy::foundations
