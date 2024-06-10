#pragma once

#include "token.hpp"
#include <concepts>
#include <cstdint>
#include <limits>

namespace matoy::syntax {

class TokenSet {
  public:
    using storage_type = uint64_t;
    static constexpr int bits = std::numeric_limits<storage_type>::digits;

    constexpr TokenSet(storage_type v = 0) : v{v} {}

    constexpr TokenSet(std::same_as<Token> auto... tokens) {
        v = (to_bits(tokens) | ...);
    }

    constexpr TokenSet operator|(const TokenSet& other) const {
        return {v | other.v};
    }

    constexpr TokenSet operator|(Token token) const {
        return {v | to_bits(token)};
    }

    constexpr bool contains(Token token) const {
        return static_cast<int>(token) < bits && (v & to_bits(token));
    }

  private:
    static constexpr storage_type to_bits(Token token) {
        return static_cast<storage_type>(1) << static_cast<int>(token);
    }

    storage_type v{};
};

namespace sets {

/// Syntax kinds that can start a statement.
inline constexpr TokenSet stmt{};

/// Syntax kinds that can start an atomic code primary.
inline constexpr TokenSet atomic_primary{Token::Ident, Token::LBrace, Token::LBracket, Token::LParen, Token::None,
                                         Token::Int,   Token::Float,  Token::Bool,     Token::Str};

/// Syntax kinds that can start a code primary.
inline constexpr TokenSet primary{atomic_primary};

/// Syntax kinds that are unary operators.
inline constexpr TokenSet unary_op{Token::Plus, Token::Minus, Token::Not};

/// Syntax kinds that are binary operators.
inline constexpr TokenSet binary_op{Token::Plus,    Token::Minus,  Token::Star,    Token::Slash,  Token::PlusEq,
                                    Token::MinusEq, Token::StarEq, Token::SlashEq, Token::ExclEq, Token::Eq,
                                    Token::EqEq,    Token::Lt,     Token::LtEq,    Token::Gt,     Token::GtEq,
                                    Token::ColonEq, Token::And,    Token::Or};

/// Syntax kinds that can start an atomic code expression.
inline constexpr TokenSet atomic_expr{atomic_primary};

/// Syntax kinds that can start a code expression.
inline constexpr TokenSet expr{primary | unary_op};

} // namespace sets

} // namespace matoy::syntax
