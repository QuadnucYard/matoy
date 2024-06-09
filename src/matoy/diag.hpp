#pragma once

#include "matoy/syntax/span.hpp"
#include <concepts>
#include <expected>
#include <string>
#include <type_traits>
#include <vector>

namespace matoy::diag {

template <class T>
using StrResult = std::expected<T, std::string>;

struct Hints {
    std::vector<std::string> v;

    Hints(std::convertible_to<std::string> auto&&... args) : v{args...} {}

    decltype(auto) operator*(this auto&& self) {
        return self.v;
    }

    decltype(auto) operator->(this auto&& self) {
        return &self.v;
    }
};

template <class T>
using HintedResult = std::expected<T, Hints>;

enum Severity { Error, Warning };

struct SourceDiagnostic {
    Severity severity;
    syntax::Span span;
    std::string message;
    std::vector<std::string> hints;

    static auto error(syntax::Span span, std::string message) -> SourceDiagnostic {
        return SourceDiagnostic{
            .severity = Severity::Error,
            .span = span,
            .message = std::move(message),
            .hints = {},
        };
    }

    auto with_hint(std::convertible_to<std::string> auto&&... args) {
        (hints.push_back(std::move(args)), ...);
    }
};

template <class T>
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

inline auto hint_error(std::string message) {
    return std::unexpected{Hints{message}};
}

inline auto source_error(syntax::Span span, std::string message) {
    return std::unexpected{std::vector<SourceDiagnostic>{SourceDiagnostic{
        .severity = Severity::Error,
        .span = span,
        .message = std::move(message),
        .hints = {},
    }}};
}

inline auto source_error(syntax::Span span, std::string message, std::convertible_to<std::string> auto&&... hints) {
    return std::unexpected{std::vector<SourceDiagnostic>{SourceDiagnostic{
        .severity = Severity::Error,
        .span = span,
        .message = std::move(message),
        .hints = {(std::move(hints), ...)},
    }}};
}

template <class T>
inline auto to_source_error(HintedResult<T>&& result, syntax::Span span) -> SourceResult<T> {
    return std::move(result).transform_error([span](auto&& err) {
        auto sd = SourceDiagnostic::error(span, std::move(err->at(0)));
        std::move(err->begin() + 1, err->end(), std::back_inserter(sd.hints));
        return std::vector<SourceDiagnostic>{std::move(sd)};
    });
}

template <class T>
inline auto to_source_error(HintedResult<T>& result, syntax::Span span) {
    return to_source_error(std::move(result), span);
}

template <class T, class E>
inline auto to_source_error(std::expected<T, E>&& result, syntax::Span span) {
    return std::move(result).transform_error(
        [span](auto&& err) { return std::vector<SourceDiagnostic>{SourceDiagnostic::error(span, std::move(err))}; });
}

template <class T, class E>
inline auto to_source_error(std::expected<T, E>& result, syntax::Span span) {
    return to_source_error(std::move(result), span);
}

} // namespace matoy::diag

namespace matoy {

template <class T, class E>
inline auto clone(std::expected<T*, E>&& result) {
    return result.transform([](auto&& val) { return *val; });
}

template <class T, class F, class E = std::invoke_result_t<F>>
inline auto ok_or_else(std::optional<T>&& opt, F&& f) -> std::expected<T, E> {
    if (opt) {
        return std::expected<T, E>{std::move(*opt)};
    } else {
        return std::unexpected<E>{std::move(std::invoke(f))};
    }
}

} // namespace matoy
