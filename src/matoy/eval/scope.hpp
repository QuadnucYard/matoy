#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/value.hpp"
#include "matoy/utils/hash.hpp"
#include "matoy/utils/ranges.hpp"
#include <concepts>
#include <format>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace matoy::eval {

class Scope {
  public:
    using key_type = std::string;
    using value_type = Value;
    using storage_type = std::unordered_map<std::string, value_type, utils::transparent_string_hash, std::equal_to<>>;
    using value_pointer = value_type*;
    using value_const_pointer = const value_type*;

    auto define(std::constructible_from<key_type> auto name, std::constructible_from<value_type> auto value) -> void {
        values.emplace(std::move(name), std::move(value));
    }

    auto get(std::string_view var) const -> std::optional<value_const_pointer> {
        if (auto it = values.find(var); it != values.end()) {
            return &it->second;
        }
        return std::nullopt;
    }

    auto get_mut(std::string_view var) -> std::optional<value_pointer> {
        if (auto it = values.find(var); it != values.end()) {
            return &it->second;
        }
        return std::nullopt;
    }

  private:
    storage_type values;
};

struct Scopes {
    std::vector<Scope> scopes;

    Scopes() : scopes{{}} {}

    auto top() -> Scope& {
        return scopes.back();
    }

    auto enter() -> void {
        scopes.push_back({});
    }

    auto exit() -> void {
        scopes.pop_back();
    }

    auto get(std::string_view var) const -> diag::HintedResult<Scope::value_const_pointer> {
        using namespace matoy::utils;
        return ok_or_else(
            ranges::find_map(scopes | std::views::reverse, [var](const Scope& scope) { return scope.get(var); }),
            [var]() { return diag::Hints{std::format("unknown variable: {}", var)}; });
    }

    auto get_mut(std::string_view var) -> diag::HintedResult<Scope::value_pointer> {
        using namespace matoy::utils;
        return ok_or_else(
            ranges::find_map(scopes | std::views::reverse, [var](Scope& scope) { return scope.get_mut(var); }),
            [var]() { return diag::Hints{std::format("unknown variable: {}", var)}; });
    }
};

} // namespace matoy::eval
