#pragma once

#include <concepts>
#include <expected>
#include <vector>

namespace matoy::diag {

struct Hints {
    std::vector<std::string> v;

    Hints(std::convertible_to<std::string> auto&&... args) : v{args...} {}
};

template <typename T>
using HintedResult = std::expected<T, Hints>;

struct SourceDiagnostic {};

template <typename T>
using SourceResult = std::expected<T, std::vector<SourceDiagnostic>>;

} // namespace matoy::diag
