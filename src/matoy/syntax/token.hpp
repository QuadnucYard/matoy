#pragma once

#include "matoy/utils/match.hpp"
#include <cstdint>

namespace matoy::syntax {

enum class Token : int8_t {
    End,
    Error,

    Space,
    LineComment,
    BlockComment,

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
    ColonEq, // :=

    LParen,   // (
    RParen,   // )
    LBracket, // [
    RBracket, // ]
    LBrace,   // {
    RBrace,   // }

    Ident,
    Int,
    Float,
    Bool,
    Str,
};

inline bool is_trivia(Token token) {
    return utils::is_in(token, Token::LineComment, Token::BlockComment, Token::Space);
}

inline bool is_error(Token token) {
    return token == Token::Error;
}

inline bool is_keyword(Token) {
    return false;
}

inline bool is_terminator(Token token) {
    return utils::is_in(token, Token::End, Token::Semicolon, Token::RBrace, Token::RParen, Token::RBracket);
}

} // namespace matoy::syntax

#ifdef PRETTY

#include <format>
#include <magic_enum.hpp>

template <>
struct std::formatter<matoy::syntax::Token> : std::formatter<char> {
    auto format(const matoy::syntax::Token& token, format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", magic_enum::enum_name(token));
    }
};

#endif
