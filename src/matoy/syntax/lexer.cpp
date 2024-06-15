#include "lexer.hpp"
#include "chars.hpp"
#include "matoy/utils/match.hpp"
#include <cstdlib>
#include <format>
#include <optional>
#include <string>
#include <string_view>

namespace matoy::syntax {

auto is_newline(char c) -> bool {
    return utils::is_in<'\n', '\r', '\0x0b', '\0x0c'>(c);
}

auto is_id_start(char c) -> bool {
    return c == '_' || is_alpha(c);
}

auto is_id_continue(char c) -> bool {
    return c == '_' || is_alnum(c);
}

auto keyword(std::string_view ident) -> std::optional<Token> {
    if (ident == "none")
        return Token::None;
    if (ident == "true")
        return Token::Bool;
    if (ident == "false")
        return Token::Bool;
    if (ident == "not")
        return Token::Not;
    if (ident == "and")
        return Token::And;
    if (ident == "or")
        return Token::Or;
    if (ident == "in")
        return Token::In;
    if (ident == "as")
        return Token::As;
    if (ident == "if")
        return Token::If;
    if (ident == "else")
        return Token::Else;
    if (ident == "for")
        return Token::For;
    if (ident == "while")
        return Token::While;
    if (ident == "break")
        return Token::Break;
    if (ident == "continue")
        return Token::Continue;
    if (ident == "return")
        return Token::Return;
    return std::nullopt;
}

struct Lexer::Impl {

    static auto next_token(Lexer& l) -> Token {
        l.err = std::nullopt;
        auto start = l.s.cursor();
        if (auto c_ = l.s.eat()) {
            auto c = *c_;
            if (is_space(c)) {
                return whitespace(l);
            } else if (c == '/') {
                if (l.s.eat_if('/')) {
                    return line_comment(l);
                } else if (l.s.eat_if('*')) {
                    return block_comment(l);
                }
            } else if (c == '*') {
                if (l.s.eat_if('/')) {
                    return l.error("unexpected end of block comment");
                }
            }
            return code(l, start, c);
        } else {
            return Token::End;
        }
    }

    static auto whitespace(Lexer& l) -> Token {
        l.s.eat_while(is_space);
        return Token::Space;
    }

    static auto line_comment(Lexer& l) -> Token {
        l.s.eat_until(is_newline);
        return Token::LineComment;
    }

    static auto block_comment(Lexer& l) -> Token {
        char state{'_'};
        int depth{1};

        for (auto c = l.s.eat(); c; c = l.s.eat()) {
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

    static auto code(Lexer& l, size_t start, char c) -> Token {
        switch (c) {
        case '0' ... '9': return number(l, start, c);
        case '.':
            if (l.s.at(is_digit)) {
                return number(l, start, c);
            }
            return Token::Dot;
        case '"': return string(l);
        case '+':
            if (l.s.eat_if('=')) {
                return Token::PlusEq;
            }
            return Token::Plus;
        case '-':
            if (l.s.eat_if('=')) {
                return Token::MinusEq;
            }
            return Token::Minus;
        case '*':
            if (l.s.eat_if('=')) {
                return Token::StarEq;
            }
            return Token::Star;
        case '/':
            if (l.s.eat_if('=')) {
                return Token::SlashEq;
            }
            return Token::Slash;
        case '!':
            if (l.s.eat_if('=')) {
                return Token::ExclEq;
            }
            return Token::Excl;
        case '=':
            if (l.s.eat_if('=')) {
                return Token::EqEq;
            }
            return Token::Eq;
        case '<':
            if (l.s.eat_if('=')) {
                return Token::LtEq;
            }
            return Token::Lt;
        case '>':
            if (l.s.eat_if('=')) {
                return Token::GtEq;
            }
            return Token::Gt;
        case ':':
            if (l.s.eat_if('=')) {
                return Token::ColonEq;
            }
            return Token::Colon;
        case '~':
            if (l.s.eat_if('=')) {
                return Token::TildeEq;
            }
            return Token::Tilde;
        case ',': return Token::Comma;
        case ';': return Token::Semicolon;
        case '(': return Token::LParen;
        case ')': return Token::RParen;
        case '[': return Token::LBracket;
        case ']': return Token::RBracket;
        case '{': return Token::LBrace;
        case '}': return Token::RBrace;
        default:
            if (is_id_start(c)) {
                return ident(l, start);
            }
            return l.error(std::format("the character `{}` is not valid in code", c));
        }
    }

    static auto ident(Lexer& l, size_t start) -> Token {
        l.s.eat_while(is_id_continue);
        auto ident = l.s.get_from(start);

        auto prev = l.s.get_before(start);
        if (!prev.ends_with('.')) {
            if (auto kw = keyword(ident)) {
                return *kw;
            }
        }

        return Token::Ident;
    }

    static auto number(Lexer& l, size_t start, char c) -> Token {
        // Handle alternative integer bases.
        int base = 10;
        if (c == '0') {
            if (l.s.eat_if('b')) {
                base = 2;
            } else if (l.s.eat_if('o')) {
                base = 8;
            } else if (l.s.eat_if('x')) {
                base = 16;
            }
            if (base != 10) {
                start = l.s.cursor();
            }
        }

        // Read the first part (integer or fractional depending on `first`).
        l.s.eat_while(base == 16 ? is_alnum : is_digit);

        // Next is the fraction part
        bool is_float = false;
        if (l.s.eat_if('.')) {
            is_float = true;
            l.s.eat_while(is_digit);
        }

        auto end = l.s.cursor();
        auto num = l.s.get(start, end);
        if (!is_float && parse_int<int64_t>(num)) {
            return Token::Int;
        } else if (base == 10 && parse_double(num)) {
            return Token::Float;
        } else {
            switch (base) {
            case 2:  return l.error(std::format("invalid binary number: 0b{}", num));
            case 8:  return l.error(std::format("invalid octal number: 0o{}", num));
            case 16: return l.error(std::format("invalid hexadecimal number: 0x{}", num));
            default: return l.error(std::format("invalid number: {}", num));
            }
        }
    }

    static auto string(Lexer& l) -> Token {
        l.s.eat_until([escaped = false](char c) mutable {
            bool stop = c == '"' && !escaped;
            escaped = c == '\\' && !escaped;
            return stop;
        });
        if (!l.s.eat_if('"')) {
            return l.error("unclosed string");
        }
        return Token::Str;
    }
};

Spanned<Token> Lexer::next() {
    auto start = this->s.cursor();
    auto token = this->next_token();
    auto end = this->s.cursor();
    return Spanned{token, {start, end}};
}

Token Lexer::next_token() {
    return Impl::next_token(*this);
}

Token Lexer::error(std::string message) {
    this->err = std::move(message);
    return Token::Error;
}

} // namespace matoy::syntax
