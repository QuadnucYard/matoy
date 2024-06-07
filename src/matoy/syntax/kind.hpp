#pragma once

#include <cstdint>

namespace matoy::syntax {

enum class SyntaxKind : int8_t {

    Dot,       // .
    Comma,     // ,
    Colon,     // :
    Semicolon, // ;

    Plus,    // +
    PlusEq,  // +=
    Minus,   // -
    MinusEq, // -=
    Star,    // *
    StarEq,  // *=
    Slash,   // /
    SlashEq, // /=
    Excl,    // !
    ExclEq,  // !=
    Eq,      // =
    EqEq,    // ==
    Lt,      // <
    LtEq,    // <=
    Gt,      //  >
    GtEq,    // >=

    LParen,   // (
    RParen,   // )
    LBracket, // [
    RBracket, // ]
    LBrace,   // {
    RBrace,   // }

    Code,
    CodeBlock,
    Parenthesized,

    Ident,
    Int,
    Float,
    Number,

    Unary,
    Binary,
    FieldAccess,
    FuncCall,
    Args,
};

} // namespace matoy::syntax
