#pragma once

#include "matoy/syntax/span.hpp"
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

enum Severity { Error, Warning };

struct SourceDiagnostic {
    Severity severity;
    syntax::Span span;
    std::string message;
    std::vector<std::string> hints;
};

template <typename T>
using SourceResult = std::expected<T, std::vector<SourceDiagnostic>>;

struct SyntaxError {
    syntax::Span span{};
    std::string message{};
    std::vector<std::string> hints{};

    // operator SourceDiagnostic() const& {
    //     return SourceDiagnostic{Severity::Error, span, message, hints};
    // }

    operator SourceDiagnostic() const&& {
        return SourceDiagnostic{Severity::Error, span, std::move(message), std::move(hints)};
    }
};

} // namespace matoy::diag
