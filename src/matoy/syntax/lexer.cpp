#include "lexer.hpp"
#include "chars.hpp"
#include "matoy/utils/match.hpp"
#include <cstdlib>
#include <format>
#include <string>

namespace matoy::syntax {

auto is_newline(char c) -> bool {
    return utils::is_in(c, '\n', '\r', '\0x0b', '\0x0c');
}

auto is_id_start(char c) -> bool {
    return c == '_' || is_alpha(c);
}

auto is_id_continue(char c) -> bool {
    return c == '_' || is_alnum(c);
}

Spanned<Token> Lexer::next() {
    auto start = this->s.cursor();
    auto token = this->next_token();
    auto end = this->s.cursor();
    return Spanned{token, {start, end}};
}

Token Lexer::next_token() {
    auto start = this->s.cursor();
    if (auto c_ = this->s.eat()) {
        auto c = *c_;
        if (is_space(c)) {
            return this->whitespace();
        } else if (c == '/') {
            if (this->s.eat_if('/')) {
                return this->line_comment();
            } else if (this->s.eat_if('*')) {
                return this->block_comment();
            }
        } else if (c == '*') {
            if (this->s.eat_if('/')) {
                return this->error("unexpected end of block comment");
            }
        }
        return this->code(start, c);
    } else {
        return Token::End;
    }
}

Token Lexer::whitespace() {
    this->s.eat_while(is_space);
    return Token::Space;
}

Token Lexer::line_comment() {
    this->s.eat_until(is_newline);
    return Token::LineComment;
}

Token Lexer::block_comment() {
    char state{'_'};
    int depth{1};

    for (auto c = this->s.eat(); c; c = this->s.eat()) {
        if (state == '*' && *c == '/') {
            if (--depth == 0) {
                break;
            }
            state = '_';
        } else if (state == '*' && *c == '/') {
            depth += 1;
            state = '_';
        } else {
            state = *c;
        }
    }

    return Token::BlockComment;
}

Token Lexer::code(size_t start, char c) {
    switch (c) {
    // case '0':
    // case '1':
    // case '2':
    // case '3':
    // case '4':
    // case '5':
    // case '6':
    // case '7':
    // case '8':
    // case '9':
    case '0' ... '9':
        return this->number(start, c);
    case '+':
        if (this->s.eat_if('=')) {
            return Token::PlusEq;
        }
        return Token::Plus;
    case '-':
        if (this->s.eat_if('=')) {
            return Token::MinusEq;
        }
        return Token::Minus;
    case '*':
        if (this->s.eat_if('=')) {
            return Token::StarEq;
        }
        return Token::Star;
    case '/':
        if (this->s.eat_if('=')) {
            return Token::SlashEq;
        }
        return Token::Slash;
    case '!':
        if (this->s.eat_if('=')) {
            return Token::ExclEq;
        }
        return Token::Excl;
    case '=':
        if (this->s.eat_if('=')) {
            return Token::EqEq;
        }
        return Token::Eq;
    case '<':
        if (this->s.eat_if('=')) {
            return Token::LtEq;
        }
        return Token::Lt;
    case '>':
        if (this->s.eat_if('=')) {
            return Token::GtEq;
        }
        return Token::Gt;
    case '(':
        return Token::LParen;
    case ')':
        return Token::RParen;
    case '[':
        return Token::LBracket;
    case ']':
        return Token::RBracket;
    case '{':
        return Token::LBrace;
    case '}':
        return Token::RBrace;
    default:
        if (is_id_start(c)) {
            return this->ident(start);
        }
        return this->error(std::format("the character `{}` is not valid in code", c));
    }
}

Token Lexer::ident(size_t) {
    this->s.eat_while(is_id_continue);
    return Token::Ident;
}

Token Lexer::number(size_t start, char c) {
    // Handle alternative integer bases.
    int base = 10;
    if (c == '0') {
        if (this->s.eat_if('b')) {
            base = 2;
        } else if (this->s.eat_if('o')) {
            base = 8;
        } else if (this->s.eat_if('x')) {
            base = 16;
        }
        if (base != 10) {
            start = this->s.cursor();
        }
    }

    // Read the first part (integer or fractional depending on `first`).
    this->s.eat_while(base == 16 ? is_alnum : is_digit);

    // Next is the fraction part
    bool is_float = false;
    if (this->s.eat_if('.')) {
        is_float = true;
        this->s.eat_while(is_digit);
    }

    auto end = this->s.cursor();
    auto num = this->s.get(start, end);
    if (!is_float && parse_int<int64_t>(num)) {
        return Token::Int;
    } else if (parse_double(num)) {
        return Token::Float;
    }

    return this->error(std::format("unknown number: {}", num));
}

Token Lexer::error(std::string message) {
    this->err = std::move(message);
    return Token::Error;
}

} // namespace matoy::syntax
