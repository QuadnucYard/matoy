#pragma once

#include "token.hpp"
#include <optional>

namespace matoy::syntax {

enum class Assoc { Left, Right };

enum class UnOp {
    Pos, // +
    Neg, // -
};

enum class BinOp {
    Add, // +
    Sub, // -
    Mul, // *
    Div, // /
    Eq,  // ==
    Neq, // !=
    Lt,  // <
    Leq, // <=
    Gt,  // >
    Geq, // >=

    Assign,     // =
    DeclAssign, // :=
    AddAssign,  // +=
    SubAssign,  // -=
    MulAssign,  // *=
    DivAssign,  // /=
};

inline auto unop_from_token(Token token) -> std::optional<UnOp> {
    switch (token) {
    case Token::Plus:  return UnOp::Pos;
    case Token::Minus: return UnOp::Neg;
    default:           return std::nullopt;
    }
}

inline auto binop_from_token(Token token) -> std::optional<BinOp> {
    switch (token) {
    case Token::Plus:    return BinOp::Add;
    case Token::PlusEq:  return BinOp::AddAssign;
    case Token::Minus:   return BinOp::Sub;
    case Token::MinusEq: return BinOp::SubAssign;
    case Token::Star:    return BinOp::Mul;
    case Token::StarEq:  return BinOp::MulAssign;
    case Token::Slash:   return BinOp::Div;
    case Token::SlashEq: return BinOp::DivAssign;
    case Token::ExclEq:  return BinOp::Neq;
    case Token::Eq:      return BinOp::Assign;
    case Token::EqEq:    return BinOp::Eq;
    case Token::Lt:      return BinOp::Lt;
    case Token::LtEq:    return BinOp::Leq;
    case Token::Gt:      return BinOp::Gt;
    case Token::GtEq:    return BinOp::Geq;
    case Token::ColonEq: return BinOp::DeclAssign;
    default:             return std::nullopt;
    }
}

inline auto precedence(UnOp op) -> int {
    switch (op) {
    case UnOp::Pos:
    case UnOp::Neg: return 7;
    }
}

inline auto precedence(BinOp op) -> int {
    switch (op) {
    case BinOp::Mul:
    case BinOp::Div:        return 6;
    case BinOp::Add:
    case BinOp::Sub:        return 5;
    case BinOp::Eq:
    case BinOp::Neq:
    case BinOp::Lt:
    case BinOp::Leq:
    case BinOp::Gt:
    case BinOp::Geq:        return 4;
    case BinOp::Assign:
    case BinOp::DeclAssign:
    case BinOp::AddAssign:
    case BinOp::SubAssign:
    case BinOp::MulAssign:
    case BinOp::DivAssign:  return 1;
    }
}

inline auto assoc(BinOp op) -> Assoc {
    switch (op) {
    case BinOp::Add:
    case BinOp::Sub:
    case BinOp::Mul:
    case BinOp::Div:
    case BinOp::Eq:
    case BinOp::Neq:
    case BinOp::Lt:
    case BinOp::Leq:
    case BinOp::Gt:
    case BinOp::Geq:        return Assoc::Left;
    case BinOp::Assign:
    case BinOp::DeclAssign:
    case BinOp::AddAssign:
    case BinOp::SubAssign:
    case BinOp::MulAssign:
    case BinOp::DivAssign:  return Assoc::Right;
    }
}

} // namespace matoy::syntax
