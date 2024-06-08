#pragma once

#include <concepts>
#include <expected>
#include <string>
#include <vector>

namespace matoy::diag {

template <typename T>
using StrResult = std::expected<T, std::string>;

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
