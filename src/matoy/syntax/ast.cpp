#include "ast.hpp"
#include "matoy/syntax/chars.hpp"
#include "matoy/syntax/kind.hpp"
#include "matoy/utils/ranges.hpp"

namespace matoy::syntax {

namespace ast {

auto Ident::get() const -> std::string_view {
    return n.as_leaf()->text;
}

auto Int::get() const -> int64_t {
    return parse_int<int64_t>(n.as_leaf()->text).value_or(0);
}

auto Float::get() const -> double {
    return parse_double(n.as_leaf()->text).value_or(0);
}

auto Bool::get() const -> bool {
    return n.as_leaf()->text == "true";
}

auto Code::exprs() const -> std::vector<Expr> {
    return n.cast_all_matches<Expr>();
}

auto CodeBlock::body() const -> Code {
    return n.cast_first_match<Code>().value();
}

auto Parenthesized::expr() const -> Expr {
    return n.cast_first_match<Expr>().value();
}

auto MatrixRow::extent() const -> size_t {
    size_t len{};
    for (auto& child : n.as_inner()->children) {
        if (child.cast<Expr>()) {
            len++;
        }
    }
    return len;
}

auto Matrix::extent() const -> size_t {
    size_t len{};
    for (auto& child : n.as_inner()->children) {
        if (child.cast<MatrixRow>()) {
            len++;
        }
    }
    return len;
}

auto Matrix::shape() const -> std::pair<size_t, size_t> {
    size_t rows{extent()};
    size_t cols{n.cast_first_match<MatrixRow>()->extent()};
    return {rows, cols};
}

auto Matrix::items() const -> std::vector<Expr> {
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

auto Unary::op() const -> UnOp {
    using namespace matoy::utils;
    return ranges::find_map(n.as_inner()->children, [](auto& node) { return unop_from_token(node.token()); })
        .value_or(UnOp::Pos);
}

auto Unary::expr() const -> Expr {
    return n.cast_last_match<Expr>().value();
}

auto Binary::op() const -> BinOp {
    using namespace matoy::utils;
    return ranges::find_map(n.as_inner()->children, [](auto& node) { return binop_from_token(node.token()); })
        .value_or(BinOp::Add);
}

auto Binary::lhs() const -> Expr {
    return n.cast_first_match<Expr>().value();
}

auto Binary::rhs() const -> Expr {
    return n.cast_last_match<Expr>().value();
}

auto FieldAccess::target() const -> Expr {
    return n.cast_first_match<Expr>().value();
}

auto FieldAccess::field() const -> Ident {
    return n.cast_last_match<Ident>().value();
}

auto Args::items() const -> std::vector<Expr> {
    return n.cast_all_matches<Expr>();
}

auto FuncCall::callee() const -> Expr {
    return n.cast_first_match<Expr>().value();
}

auto FuncCall::args() const -> Args {
    return n.cast_last_match<Args>().value();
}

auto Conditional::condition() const -> Expr {
    return n.cast_first_match<Expr>().value();
}

auto Conditional::if_body() const -> Expr {
    return n.cast_nth_match<Expr>(1).value();
}

auto Conditional::else_body() const -> std::optional<Expr> {
    return n.cast_nth_match<Expr>(2);
}

auto WhileLoop::condition() const -> Expr {
    return n.cast_first_match<Expr>().value();
}

auto WhileLoop::body() const -> Expr {
    return n.cast_last_match<Expr>().value();
}

auto FuncReturn::body() const -> std::optional<Expr> {
    return n.cast_first_match<Expr>();
}

} // namespace ast

#define IMPL_TYPED0($name)                                                                                             \
    template <>                                                                                                        \
    auto from_untyped<ast::$name>(const SyntaxNode& node) -> std::optional<ast::$name> {                               \
        if (node.token() == Token::$name) {                                                                            \
            return ast::$name{node};                                                                                   \
        } else {                                                                                                       \
            return std::nullopt;                                                                                       \
        }                                                                                                              \
    }

#define IMPL_TYPED($name)                                                                                              \
    template <>                                                                                                        \
    auto from_untyped<ast::$name>(const SyntaxNode& node) -> std::optional<ast::$name> {                               \
        if (node.kind() == SyntaxKind::$name) {                                                                        \
            return ast::$name{node};                                                                                   \
        } else {                                                                                                       \
            return std::nullopt;                                                                                       \
        }                                                                                                              \
    }

IMPL_TYPED0(Ident)
IMPL_TYPED0(None)
IMPL_TYPED0(Int)
IMPL_TYPED0(Float)
IMPL_TYPED0(Bool)

IMPL_TYPED(Code)
IMPL_TYPED(CodeBlock)
IMPL_TYPED(Parenthesized)
IMPL_TYPED(MatrixRow)
IMPL_TYPED(Matrix)
IMPL_TYPED(Unary)
IMPL_TYPED(Binary)
IMPL_TYPED(FieldAccess)
IMPL_TYPED(FuncCall)
IMPL_TYPED(Args)

IMPL_TYPED(Conditional)
IMPL_TYPED(WhileLoop)
IMPL_TYPED(ForLoop)
IMPL_TYPED(LoopBreak)
IMPL_TYPED(LoopContinue)
IMPL_TYPED(FuncReturn)

#undef IMPL_TYPED0
#undef IMPL_TYPED

template <>
auto from_untyped<ast::Expr>(const SyntaxNode& node) -> std::optional<ast::Expr> {
    if (auto leaf = node.as_leaf()) {
        switch (leaf->token) {
        case Token::Ident: return ast::Ident{node};
        case Token::None:  return ast::None{node};
        case Token::Int:   return ast::Int{node};
        case Token::Float: return ast::Float{node};
        case Token::Bool:  return ast::Bool{node};
        default:           return std::nullopt;
        }
    }
    if (auto inner = node.as_inner()) {
        switch (inner->kind) {
        case SyntaxKind::CodeBlock:     return ast::CodeBlock{node};
        case SyntaxKind::Parenthesized: return ast::Parenthesized{node};
        case SyntaxKind::Matrix:        return ast::Matrix{node};
        case SyntaxKind::Unary:         return ast::Unary{node};
        case SyntaxKind::Binary:        return ast::Binary{node};
        case SyntaxKind::FieldAccess:   return ast::FieldAccess{node};
        case SyntaxKind::FuncCall:      return ast::FuncCall{node};
        case SyntaxKind::Conditional:   return ast::Conditional{node};
        case SyntaxKind::WhileLoop:     return ast::WhileLoop{node};
        case SyntaxKind::ForLoop:       return ast::ForLoop{node};
        case SyntaxKind::LoopBreak:     return ast::LoopBreak{node};
        case SyntaxKind::LoopContinue:  return ast::LoopContinue{node};
        case SyntaxKind::FuncReturn:    return ast::FuncReturn{node};
        default:                        return std::nullopt;
        }
    }
    throw "unimplemented!";
}

auto get_span(const ast::Expr& self) -> Span {
    return self.visit([](auto& v) { return v.span(); });
}

} // namespace matoy::syntax
