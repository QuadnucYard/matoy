#pragma once

#include "kind.hpp"
#include "matoy/utils/ranges.hpp"
#include "span.hpp"
#include "token.hpp"
#include <optional>
#include <ranges>
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
    size_t len{};
    Span span{};
    size_t descendants{};
    std::vector<SyntaxNode> children;

    InnerNode(SyntaxKind kind, std::vector<SyntaxNode> children);
};

template <typename T>
auto from_untyped(const SyntaxNode& node) -> std::optional<T>;

struct SyntaxNode {
    std::variant<LeafNode, InnerNode> v;

    // Create a new leaf node.
    static SyntaxNode leaf(Token token, std::string_view text, Span span = {}) {
        return {LeafNode{token, text, span}};
    }

    // Create a new inner node with children.
    static SyntaxNode inner(SyntaxKind kind, std::vector<SyntaxNode> children) {
        return {InnerNode{kind, std::move(children)}};
    }

    bool is_leaf() const {
        return std::holds_alternative<LeafNode>(v);
    }

    bool is_inner() const {
        return std::holds_alternative<InnerNode>(v);
    }

    const LeafNode* as_leaf() const {
        return std::get_if<LeafNode>(&v);
    }

    const InnerNode* as_inner() const {
        return std::get_if<InnerNode>(&v);
    }

    Span span() const;

    size_t len() const;

    size_t descendants() const;

    Token token() const;

    SyntaxKind kind() const;

    /// Whether the node can be cast to the given AST node.
    template <typename T>
    bool is() const {
        return false;
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
};

} // namespace matoy::syntax
