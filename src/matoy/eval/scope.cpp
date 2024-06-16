#include "scope.hpp"
#include "matoy/utils/ranges.hpp"

namespace matoy::eval {

auto Scope::get(std::string_view var) const -> std::optional<value_const_pointer> {
    if (auto it = values.find(var); it != values.end()) {
        return &it->second;
    }
    return std::nullopt;
}

auto Scope::get_mut(std::string_view var) -> std::optional<value_pointer> {
    if (auto it = values.find(var); it != values.end()) {
        return &it->second;
    }
    return std::nullopt;
}

auto Scopes::get(std::string_view var) const -> diag::HintedResult<Scope::value_const_pointer> {
    using namespace matoy::utils;
    return ok_or_else(
        ranges::find_map(scopes | std::views::reverse, [var](const Scope& scope) { return scope.get(var); }),
        [var]() { return diag::Hints{std::format("unknown variable: {}", var)}; });
}

auto Scopes::get_mut(std::string_view var) -> diag::HintedResult<Scope::value_pointer> {
    using namespace matoy::utils;
    return ok_or_else(
        ranges::find_map(scopes | std::views::reverse, [var](Scope& scope) { return scope.get_mut(var); }),
        [var]() { return diag::Hints{std::format("unknown variable: {}", var)}; });
}

} // namespace matoy::eval
