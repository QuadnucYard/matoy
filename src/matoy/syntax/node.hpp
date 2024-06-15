#pragma once

#include "kind.hpp"
#include "matoy/diag.hpp"
#include "matoy/utils/ranges.hpp"
#include "span.hpp"
#include "token.hpp"
#include <concepts>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace matoy::syntax {

struct SyntaxNode;

struct LeafNode {
    Token token;
    std::string_view text;
    Span span{};

    size_t len() const {
        return text.length();
    }
};

struct InnerNode {
    SyntaxKind kind;
    std::string_view text;
    Span span{};
    size_t descendants{};
    bool erroneous{};
    std::vector<SyntaxNode> children;

    InnerNode(SyntaxKind kind, std::string_view text, std::vector<SyntaxNode> children);

    InnerNode(InnerNode&&) = default;
    InnerNode& operator=(InnerNode&&) = default;

    size_t len() const {
        return text.length();
    }

  private:
    InnerNode(const InnerNode&) = default;
    InnerNode& operator=(const InnerNode&) = default;
};

struct ErrorNode {
    std::string text;
    diag::SyntaxError error;

    ErrorNode(std::string text, std::string message, Span span)
        : text{std::move(text)}, error{.span = span, .message = std::move(message)} {}

    size_t len() const {
        return text.length();
    }

    void hint(std::string message) {
        error.hints.push_back(std::move(message));
    }
};

template <typename T>
auto from_untyped(const SyntaxNode& node) -> std::optional<T> = delete;

struct SyntaxNode {
    std::variant<LeafNode, InnerNode, ErrorNode> v;

    // Create a new leaf node.
    static SyntaxNode leaf(Token token, std::string_view text, Span span) {
        return {LeafNode{token, text, span}};
    }

    // Create a new inner node with children.
    static SyntaxNode inner(SyntaxKind kind, std::string_view text, std::vector<SyntaxNode> children) {
        return {InnerNode{kind, text, std::move(children)}};
    }

    // Create a new error node.
    static SyntaxNode error(auto&& text, auto&& message, Span span)
        requires requires {
            std::constructible_from<std::string, decltype(text)>;
            std::constructible_from<std::string, decltype(message)>;
        }
    {
        return {ErrorNode{std::string{std::move(text)}, std::string{std::move(message)}, span}};
    }

    bool is_leaf() const {
        return std::holds_alternative<LeafNode>(v);
    }

    bool is_inner() const {
        return std::holds_alternative<InnerNode>(v);
    }

    bool is_error() const {
        return std::holds_alternative<ErrorNode>(v);
    }

    const LeafNode* as_leaf() const {
        return std::get_if<LeafNode>(&v);
    }

    const InnerNode* as_inner() const {
        return std::get_if<InnerNode>(&v);
    }

    const ErrorNode* as_error() const {
        return std::get_if<ErrorNode>(&v);
    }

    Span span() const;

    size_t len() const;

    size_t descendants() const;

    Token token() const;

    SyntaxKind kind() const;

    bool erroneous() const;

    bool stores_error() const;

    bool empty() const {
        return len() == 0;
    }

    std::string_view text() const;

    // std::string extract_text();

    std::vector<diag::SyntaxError> errors() const;

    /// Convert the child to another kind.
    void convert_to_kind(SyntaxKind kind);

    /// Convert the child to an error, if it isn't already one.
    void convert_to_error(auto&& message) {
        if (!stores_error()) {
            v = ErrorNode{std::string{text()}, std::string{std::move(message)}, span()};
        }
    }

    /// Convert the child to an error stating that the given thing was
    /// expected, but the current kind was found.
    void expected_token(std::string_view expected) {
        auto token = this->token();
        convert_to_error(std::format("expected {}, found {}", expected, get_name(token)));
        // TODO check is_keyword
    }

    void unexpected() {
        auto token = this->token();
        convert_to_error(std::format("unexpected {}", get_name(token)));
    }

    /// Whether the node can be cast to the given AST node.
    template <typename T>
    bool is() const {
        return cast<T>().has_value();
    }

    /// Try to convert the node to a typed AST node.
    template <typename T>
    std::optional<T> cast() const {
        return from_untyped<T>(*this);
    }

    /// Cast the first child that can cast to the AST type `T`.
    template <typename T>
    std::optional<T> cast_first_match() const {
        using namespace utils;
        return ranges::find_map(as_inner()->children, &SyntaxNode::cast<T>);
    }

    /// Cast the last child that can cast to the AST type `T`.
    template <typename T>
    std::optional<T> cast_last_match() const {
        using namespace utils;
        return ranges::find_map(as_inner()->children | std::views::reverse, &SyntaxNode::cast<T>);
    }

    /// Cast the nth child that can cast to the AST type `T`.
    template <typename T>
    std::optional<T> cast_nth_match(size_t n) const {
        size_t skipped{};
        for (auto& ch : as_inner()->children) {
            if (auto v = ch.cast<T>()) {
                if (skipped++ == n) {
                    return *v;
                }
            }
        }
        return std::nullopt;
    }

    template <typename T>
    std::vector<T> cast_all_matches() const {
        std::vector<T> res;
        for (auto& child : as_inner()->children) {
            if (auto e = child.cast<T>()) {
                res.push_back(*e);
            }
        }
        return res;
    }
};

} // namespace matoy::syntax
