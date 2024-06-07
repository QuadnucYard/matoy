#pragma once

#include "kind.hpp"
#include "span.hpp"
#include "token.hpp"
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

struct SyntaxNode {
    std::variant<LeafNode, InnerNode> v;

    // Create a new leaf node.
    static SyntaxNode leaf(Token token, std::string_view text) {
        return {LeafNode{token, text}};
    }

    // Create a new inner node with children.
    static SyntaxNode inner(SyntaxKind kind, std::vector<SyntaxNode> children) {
        return {InnerNode{kind, std::move(children)}};
    }

    size_t len() const;

    size_t descendants() const;

    Token token() const;
};

} // namespace matoy::syntax
