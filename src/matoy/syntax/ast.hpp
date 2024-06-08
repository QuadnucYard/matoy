#pragma once

#include "matoy/syntax/chars.hpp"
#include "matoy/syntax/kind.hpp"
#include "matoy/syntax/node.hpp"
#include "matoy/syntax/op.hpp"
#include "matoy/utils/ranges.hpp"
#include <optional>
#include <string_view>
#include <variant>

namespace matoy::syntax {

struct AstNode {
    const SyntaxNode& n;
};

namespace ast {

struct CodeBlock : AstNode {
    auto exprs() const;
};

struct Ident : AstNode {
    auto get() const -> std::string_view {
        return n.as_leaf()->text;
    }
};

struct Int : AstNode {
    auto get() const -> int64_t {
        return parse_int<int64_t>(n.as_leaf()->text).value_or(0);
    }
};

struct Float : AstNode {
    auto get() const -> double {
        return parse_double(n.as_leaf()->text).value_or(0);
    }
};

struct Parenthesized;
struct MatrixRow;
struct Matrix;
struct Unary;
struct Binary;
struct FieldAccess;
struct FuncCall;
using Expr = std::variant<CodeBlock, Ident, Int, Float, Parenthesized, Matrix, Unary, Binary, FieldAccess, FuncCall>;

struct Parenthesized : AstNode {
    auto expr() const -> Expr;
};

struct MatrixRow : AstNode {
    auto extent() const -> size_t;
};

struct Matrix : AstNode {
    auto extent() const -> size_t;

    auto shape() const -> std::pair<size_t, size_t>;

    auto items() const;
};

struct FieldAccess : AstNode {
    auto target() const -> Expr;

    auto field() const -> Ident;
};

struct FuncCall : AstNode {};

struct Args : AstNode {};

struct Unary : AstNode {
    auto op() const -> UnOp {
        using namespace matoy::utils;
        return ranges::find_map(n.as_inner()->children, [](auto& node) { return unop_from_token(node.token()); })
            .value_or(UnOp::Pos);
    }

    auto expr() const -> Expr;
};

struct Binary : AstNode {
    auto op() const -> BinOp {
        using namespace matoy::utils;
        return ranges::find_map(n.as_inner()->children, [](auto& node) { return binop_from_token(node.token()); })
            .value_or(BinOp::Add);
    }

    auto lhs() const -> Expr {
        return n.cast_first_match<Expr>().value();
    }

    auto rhs() const -> Expr {
        return n.cast_last_match<Expr>().value();
    }
};

inline auto CodeBlock::exprs() const {
    // return n.as_inner()->children | std::views::transform(&SyntaxNode::cast<Expr>) |
    //        std::views::filter([](auto x) { return x; });
    // return n.as_inner()->children | std::views::transform([](auto& x) -> auto { return x.template cast<Expr>(); }) |
    //        std::views::filter([](auto x) { return x.has_value(); });
    std::vector<Expr> res;
    for (auto& child : n.as_inner()->children) {
        if (auto e = child.cast<Expr>()) {
            res.push_back(*e);
        }
    }
    return res;
}

inline auto Parenthesized::expr() const -> Expr {
    return n.cast_first_match<Expr>().value();
}

inline auto MatrixRow::extent() const -> size_t {
    size_t len{};
    for (auto& child : n.as_inner()->children) {
        if (child.cast<Expr>()) {
            len++;
        }
    }
    return len;
}

inline auto Matrix::extent() const -> size_t {
    size_t len{};
    for (auto& child : n.as_inner()->children) {
        if (child.cast<MatrixRow>()) {
            len++;
        }
    }
    return len;
}

inline auto Matrix::shape() const -> std::pair<size_t, size_t> {
    size_t rows{extent()};
    size_t cols{n.cast_first_match<MatrixRow>()->extent()};
    return {rows, cols};
}

inline auto Matrix::items() const {
    std::vector<Expr> res;
    for (auto& ch : n.as_inner()->children) {
        if (auto row = ch.cast<MatrixRow>()) {
            for (auto& item : row->n.as_inner()->children) {
                if (auto it = item.cast<Expr>()) {
                    res.push_back(*it);
                }
            }
        }
    }
    return res;
}

inline auto Unary::expr() const -> Expr {
    return n.cast_last_match<Expr>().value();
}

inline auto FieldAccess::target() const -> Expr {
    return n.cast_first_match<Expr>().value();
}

inline auto FieldAccess::field() const -> Ident {
    return n.cast_last_match<Ident>().value();
}

} // namespace ast

#define IMPL_TYPED0($name)                                                                                             \
    template <>                                                                                                        \
    inline auto from_untyped<ast::$name>(const SyntaxNode& node) -> std::optional<ast::$name> {                        \
        if (node.token() == Token::$name) {                                                                            \
            return ast::$name{node};                                                                                   \
        } else {                                                                                                       \
            return std::nullopt;                                                                                       \
        }                                                                                                              \
    }

#define IMPL_TYPED($name)                                                                                              \
    template <>                                                                                                        \
    inline auto from_untyped<ast::$name>(const SyntaxNode& node) -> std::optional<ast::$name> {                        \
        if (node.kind() == SyntaxKind::$name) {                                                                        \
            return ast::$name{node};                                                                                   \
        } else {                                                                                                       \
            return std::nullopt;                                                                                       \
        }                                                                                                              \
    }

IMPL_TYPED0(Ident)
IMPL_TYPED0(Int)
IMPL_TYPED0(Float)

IMPL_TYPED(CodeBlock)
IMPL_TYPED(Parenthesized)
IMPL_TYPED(MatrixRow)
IMPL_TYPED(Matrix)
IMPL_TYPED(Unary)
IMPL_TYPED(Binary)
IMPL_TYPED(FieldAccess)
IMPL_TYPED(FuncCall)

#undef IMPL_TYPED0
#undef IMPL_TYPED

template <>
inline auto from_untyped<ast::Expr>(const SyntaxNode& node) -> std::optional<ast::Expr> {
    if (auto leaf = node.as_leaf()) {
        switch (leaf->token) {
        case Token::Ident:
            return ast::Ident{node};
        case Token::Int:
            return ast::Int{node};
        case Token::Float:
            return ast::Float{node};
        default:
            return std::nullopt;
        }
    }
    if (auto inner = node.as_inner()) {
        switch (inner->kind) {
        case SyntaxKind::CodeBlock:
            return ast::CodeBlock{node};
        case SyntaxKind::Parenthesized:
            return ast::Parenthesized{node};
        // case SyntaxKind::MatrixRow:
        //     return ast::MatrixRow{node};
        case SyntaxKind::Matrix:
            return ast::Matrix{node};
        case SyntaxKind::Unary:
            return ast::Unary{node};
        case SyntaxKind::Binary:
            return ast::Binary{node};
        case SyntaxKind::FieldAccess:
            return ast::FieldAccess{node};
        case SyntaxKind::FuncCall:
            return ast::FuncCall{node};
        default:
            return std::nullopt;
        }
    }
    throw "unimplemented!";
}

} // namespace matoy::syntax
