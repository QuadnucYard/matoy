#pragma once

#include <cstdint>
#include <string_view>

namespace matoy::syntax {

enum class SyntaxKind : int8_t {
    Error,

    CodeBlock,
    Parenthesized,

    Matrix,
    MatrixRow,

    Unary,
    Binary,
    FieldAccess,
    FuncCall,
    Args,
};

inline auto get_name(SyntaxKind self) -> std::string_view {
    switch (self) {
    case SyntaxKind::Error:         return "error";
    case SyntaxKind::CodeBlock:     return "code block";
    case SyntaxKind::Parenthesized: return "group";
    case SyntaxKind::Matrix:        return "matrix expression";
    case SyntaxKind::MatrixRow:     return "matrix row expression";
    case SyntaxKind::Unary:         return "unary expression";
    case SyntaxKind::Binary:        return "binary expression";
    case SyntaxKind::FieldAccess:   return "field access";
    case SyntaxKind::FuncCall:      return "function call";
    case SyntaxKind::Args:          return "call arguments";
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
