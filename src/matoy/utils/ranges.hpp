#pragma once

#include <iterator>
#include <ranges>

namespace matoy::utils::ranges {

struct find_map_fn {
    template <std::input_iterator I, std::sentinel_for<I> S, class Proj>
    constexpr auto operator()(I first, S last, Proj proj) const -> std::projected<I, Proj>::value_type {
        for (; first != last; ++first)
            if (auto p = std::invoke(proj, *first))
                return p;
        return {};
    }

    template <std::ranges::input_range R, class Proj>
    constexpr auto operator()(R&& r, Proj proj) const -> std::projected<std::ranges::iterator_t<R>, Proj>::value_type {
        return (*this)(std::ranges::begin(r), std::ranges::end(r), std::ref(proj));
    }
};

inline constexpr find_map_fn find_map;

} // namespace matoy::utils::ranges
