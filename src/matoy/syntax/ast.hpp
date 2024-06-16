#pragma once

#include "ast_fwd.hpp"
#include "node.hpp"
#include "op.hpp"
#include <optional>
#include <string_view>
#include <utility>

namespace matoy::syntax {

struct AstNode {
    const SyntaxNode& n;

    auto span() const -> Span {
        return n.span();
    }
};

namespace ast {

struct Ident : AstNode {
    auto get() const -> std::string_view;
};

struct None : AstNode {};

struct Int : AstNode {
    auto get() const -> int64_t;
};

struct Float : AstNode {
    auto get() const -> double;
};

struct Bool : AstNode {
    auto get() const -> bool;
};

struct Code : AstNode {
    auto exprs() const -> std::vector<Expr>;
};

struct CodeBlock : AstNode {
    auto body() const -> Code;
};

struct Parenthesized : AstNode {
    auto expr() const -> Expr;
};

struct MatrixRow : AstNode {
    auto extent() const -> size_t;
};

struct Matrix : AstNode {
    auto extent() const -> size_t;

    auto shape() const -> std::pair<size_t, size_t>;

    auto items() const -> std::vector<Expr>;
};

struct FieldAccess : AstNode {
    auto target() const -> Expr;

    auto field() const -> Ident;
};

struct Args : AstNode {
    auto items() const -> std::vector<Expr>;
};

struct FuncCall : AstNode {
    auto callee() const -> Expr;

    auto args() const -> Args;
};

struct Unary : AstNode {
    auto op() const -> UnOp;

    auto expr() const -> Expr;
};

struct Binary : AstNode {
    auto op() const -> BinOp;

    auto lhs() const -> Expr;

    auto rhs() const -> Expr;
};

struct Conditional : AstNode {
    auto condition() const -> Expr;

    auto if_body() const -> Expr;

    auto else_body() const -> std::optional<Expr>;
};

struct WhileLoop : AstNode {
    auto condition() const -> Expr;

    auto body() const -> Expr;
};

struct ForLoop : AstNode {};

struct LoopBreak : AstNode {};

struct LoopContinue : AstNode {};

struct FuncReturn : AstNode {
    auto body() const -> std::optional<Expr>;
};

} // namespace ast

#define IMPL_TYPED0($name)                                                                                             \
    template <> auto from_untyped<ast::$name>(const SyntaxNode& node) -> std::optional<ast::$name>;

#define IMPL_TYPED($name)                                                                                              \
    template <> auto from_untyped<ast::$name>(const SyntaxNode& node) -> std::optional<ast::$name>;

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

template <> auto from_untyped<ast::Expr>(const SyntaxNode& node) -> std::optional<ast::Expr>;

inline auto get_span(const AstNode& self) -> Span {
    return self.span();
}

auto get_span(const ast::Expr& self) -> Span;

} // namespace matoy::syntax
