#pragma once

#include <cstdint>

namespace matoy::syntax {

enum class SyntaxKind : int8_t {
    Error,

    CodeBlock,
    Parenthesized,

    Unary,
    Binary,
    FieldAccess,
    FuncCall,
    Args,
};

} // namespace matoy::syntax
