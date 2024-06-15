#pragma once

#include <cstdint>
#include <string_view>

namespace matoy::syntax {

enum class SyntaxKind : int8_t {
    Error,

    Code,
    CodeBlock,
    Parenthesized,

    Matrix,
    MatrixRow,

    Unary,
    Binary,
    FieldAccess,
    FuncCall,
    Args,

    Conditional,
    WhileLoop,
    ForLoop,

    LoopBreak,
    LoopContinue,
    FuncReturn,
};

inline auto get_name(SyntaxKind self) -> std::string_view {
    switch (self) {
    case SyntaxKind::Error:         return "error";
    case SyntaxKind::Code:          return "code";
    case SyntaxKind::CodeBlock:     return "code block";
    case SyntaxKind::Parenthesized: return "group";
    case SyntaxKind::Matrix:        return "matrix expression";
    case SyntaxKind::MatrixRow:     return "matrix row expression";
    case SyntaxKind::Unary:         return "unary expression";
    case SyntaxKind::Binary:        return "binary expression";
    case SyntaxKind::FieldAccess:   return "field access";
    case SyntaxKind::FuncCall:      return "function call";
    case SyntaxKind::Args:          return "call arguments";
    case SyntaxKind::Conditional:   return "`if` expression";
    case SyntaxKind::WhileLoop:     return "while-loop expression";
    case SyntaxKind::ForLoop:       return "for-loop expression";
    case SyntaxKind::LoopBreak:     return "`break` expression";
    case SyntaxKind::LoopContinue:  return "`continue` expression";
    case SyntaxKind::FuncReturn:    return "`return` expression";
    }
}

} // namespace matoy::syntax

#ifdef PRETTY

#include <format>
#include <magic_enum.hpp>

template <>
struct std::formatter<matoy::syntax::SyntaxKind> : std::formatter<char> {
    auto format(const matoy::syntax::SyntaxKind& kind, format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", magic_enum::enum_name(kind));
    }
};

#endif
