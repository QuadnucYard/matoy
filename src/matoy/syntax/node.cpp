#include "node.hpp"
#include "matoy/utils/match.hpp"

namespace matoy::syntax {

using namespace matoy::utils;

size_t SyntaxNode::len() const {
    return v.visit(overloaded{
        [](const LeafNode& leaf) { return leaf.len(); },
        [](const InnerNode& inner) { return inner.len; },
    });
}

size_t SyntaxNode::descendants() const {
    return v.visit(overloaded{
        [](const LeafNode&) { return 1uz; },
        [](const InnerNode& inner) { return inner.descendants; },
    });
}

Token SyntaxNode::token() const {
    return v.visit(overloaded{
        [](const LeafNode& leaf) { return leaf.token; },
        [](const InnerNode&) { return Token::Error; },
    });
}

InnerNode::InnerNode(SyntaxKind kind, std::vector<SyntaxNode> children) : kind{kind}, children{std::move(children)} {
    for (auto& child : this->children) {
        len += child.len();
        descendants += child.descendants();
    }
}

} // namespace matoy::syntax
