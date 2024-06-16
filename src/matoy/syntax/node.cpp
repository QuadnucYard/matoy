#include "node.hpp"
#include "matoy/syntax/kind.hpp"
#include "matoy/utils/match.hpp"
#include <ranges>
#include <string_view>

namespace matoy::syntax {

using namespace matoy::utils;

Span SyntaxNode::span() const {
    return v.visit(overloaded{
        [](const LeafNode& leaf) { return leaf.span; },
        [](const InnerNode& inner) { return inner.span; },
        [](const ErrorNode& error) { return error.error.span; },
    });
}

size_t SyntaxNode::len() const {
    return v.visit([](const auto& n) { return n.len(); });
}

size_t SyntaxNode::descendants() const {
    return v.visit(overloaded{
        [](const LeafNode&) { return 1uz; },
        [](const InnerNode& inner) { return inner.descendants; },
        [](const ErrorNode&) { return 1uz; },
    });
}

Token SyntaxNode::token() const {
    return v.visit(overloaded{
        [](const LeafNode& leaf) { return leaf.token; },
        [](const InnerNode&) { return Token::Error; },
        [](const ErrorNode&) { return Token::Error; },
    });
}

SyntaxKind SyntaxNode::kind() const {
    return v.visit(overloaded{
        [](const LeafNode&) { return SyntaxKind::Error; },
        [](const InnerNode& inner) { return inner.kind; },
        [](const ErrorNode&) { return SyntaxKind::Error; },
    });
}

bool SyntaxNode::erroneous() const {
    return v.visit(overloaded{
        [](const LeafNode&) { return false; },
        [](const InnerNode& inner) { return inner.erroneous; },
        [](const ErrorNode&) { return true; },
    });
}

bool SyntaxNode::stores_error() const {
    return v.visit(overloaded{
        [](const LeafNode& leaf) { return leaf.token == Token::Error; },
        [](const InnerNode& inner) { return inner.kind == SyntaxKind::Error; },
        [](const ErrorNode&) { return true; },
    });
}

std::string_view SyntaxNode::text() const {
    return v.visit<std::string_view>([](const auto& n) { return n.text; });
}

/* std::string SyntaxNode::extract_text() {
    return v.visit<std::string>(overloaded{
        [](LeafNode& leaf) { return std::string(leaf.text); },
        [](InnerNode&) { return ""; },
        [](ErrorNode& error) { return error.text; },
    });
} */

std::vector<diag::SyntaxError> SyntaxNode::errors() const {
    if (!erroneous()) {
        return {};
    }
    if (auto err = std::get_if<ErrorNode>(&v)) {
        return {err->error};
    } else if (auto inner = std::get_if<InnerNode>(&v)) {
        return inner->children                                                   //
               | std::views::filter([](auto& node) { return node.erroneous(); }) //
               | std::views::transform([](auto& node) { return node.errors(); }) //
               | std::views::join                                                //
               | std::ranges::to<std::vector>();                                 //
    } else {
        return {};
    }
}

void SyntaxNode::convert_to_kind(SyntaxKind kind) {
    v.visit(overloaded{
        [](LeafNode&) {},
        [kind](InnerNode& inner) { inner.kind = kind; },
        [](ErrorNode&) { std::abort(); },
    });
}

void SyntaxNode::expected_token(std::string_view expected) {
    auto token = this->token();
    convert_to_error(std::format("expected {}, found {}", expected, get_name(token)));
    // TODO check is_keyword
}

void SyntaxNode::unexpected() {
    auto token = this->token();
    convert_to_error(std::format("unexpected {}", get_name(token)));
}

InnerNode::InnerNode(SyntaxKind kind, std::string_view text, std::vector<SyntaxNode> children)
    : kind{kind}, text{text}, span{}, children{std::move(children)} {
    for (auto& child : this->children) {
        descendants += child.descendants();
        erroneous |= child.erroneous();

        if (!child.erroneous()) {
            auto subspan = child.span();
            if (span.end == 0) {
                span.start = subspan.start;
            }
            span.end = subspan.end;
        }
    }
}

} // namespace matoy::syntax
