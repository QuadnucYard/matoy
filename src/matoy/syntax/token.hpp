#pragma once

#include "matoy/utils/match.hpp"
#include <cstdint>
#include <string_view>

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
    Tilde,   // ~
    TildeEq, // ~=

    LParen,   // (
    RParen,   // )
    LBracket, // [
    RBracket, // ]
    LBrace,   // {
    RBrace,   // }

    None,
    Not,
    And,
    Or,

    Ident,
    Int,
    Float,
    Bool,
    Str,
};

inline bool is_trivia(Token self) {
    return utils::is_in<Token::LineComment, Token::BlockComment, Token::Space>(self);
}

inline bool is_error(Token self) {
    return self == Token::Error;
}

inline bool is_keyword(Token self) {
    return utils::is_in<Token::None, Token::Not, Token::And, Token::Or>(self);
}

inline bool is_terminator(Token self) {
    return utils::is_in<Token::End, Token::Semicolon, Token::RBrace, Token::RParen, Token::RBracket>(self);
}

inline auto get_name(Token self) -> std::string_view {
    switch (self) {
    case Token::End:   return "end of tokens";
    case Token::Error: return "syntax error";

    case Token::Space:        return "space";
    case Token::LineComment:  return "line comment";
    case Token::BlockComment: return "block comment";

    case Token::Dot:       return "dot";
    case Token::Comma:     return "comma";
    case Token::Colon:     return "colon";
    case Token::Semicolon: return "semicolon";

    case Token::Plus:    return "plus";
    case Token::PlusEq:  return "add-assign operator";
    case Token::Minus:   return "minus";
    case Token::MinusEq: return "subtract-assign operator";
    case Token::Star:    return "star";
    case Token::StarEq:  return "multiply-assign operator";
    case Token::Slash:   return "slash";
    case Token::SlashEq: return "divide-assign operator";
    case Token::Excl:    return "not";
    case Token::ExclEq:  return "inequality operator";
    case Token::Eq:      return "equals sign";
    case Token::EqEq:    return "equality operator";
    case Token::Lt:      return "less-than operator";
    case Token::LtEq:    return "less-than or equal operator";
    case Token::Gt:      return "greater-than operator";
    case Token::GtEq:    return "greater-than or equal operator";
    case Token::ColonEq: return "declaration-assign operator";
    case Token::Tilde:   return "tilde";
    case Token::TildeEq: return "approximate equality operator";

    case Token::LParen:   return "opening paren";
    case Token::RParen:   return "closing paren";
    case Token::LBracket: return "opening bracket";
    case Token::RBracket: return "closing bracket";
    case Token::LBrace:   return "opening brace";
    case Token::RBrace:   return "closing brace";

    case Token::None: return "none";
    case Token::Not:  return "not";
    case Token::And:  return "and";
    case Token::Or:   return "or";

    case Token::Ident: return "identifier";
    case Token::Int:   return "integer";
    case Token::Float: return "float";
    case Token::Bool:  return "boolean";
    case Token::Str:   return "string";
    }
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
