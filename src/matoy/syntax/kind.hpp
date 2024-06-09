#pragma once

#include <cstdint>

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
