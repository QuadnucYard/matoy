#include "parser.hpp"
#include "matoy/syntax/kind.hpp"
#include "matoy/syntax/op.hpp"
#include "token_set.hpp"
#include <algorithm>
#include <concepts>

namespace matoy::syntax {

struct Parser::Impl {

    static auto code(Parser& p, std::predicate<Parser&> auto stop) -> void {
        auto m = p.marker();
        code_exprs(p, stop);
        p.reduce(m, SyntaxKind::CodeBlock);
    }

    static auto code_exprs(Parser& p, std::predicate<Parser&> auto stop) -> void {
        while (!p.end() && !stop(p)) {
            if (p.at(sets::expr)) {
                code_expr(p);
                if (!p.end() && !stop(p) && !p.eat_if(Token::Semicolon)) {
                    // expect
                }
            } else {
                // unexpected
            }
        }
    }

    static auto code_expr(Parser& p) -> void {
        return code_expr_prec(p, false, 0);
    }

    static auto code_expr_prec(Parser& p, bool atomic, int min_prec) -> void {
        auto m{p.marker()};
        if (!atomic && p.at(sets::unary_op)) {
            auto op = *unop_from_token(p.current);
            p.eat();
            code_expr_prec(p, atomic, precedence(op));
            p.reduce(m, SyntaxKind::Unary);
        } else {
            code_primary(p, atomic);
        }

        while (true) {
            if (p.directly_at(Token::LParen) || p.directly_at(Token::LBracket)) {
                throw "unimplemented!";
                // args(p);
                // p.wrap(m, SyntaxKind::FuncCall);
                // continue;
            }
            auto at_field_or_method = p.directly_at(Token::Dot) && Lexer{p.lexer}.next_token() == Token::Ident;
            if (atomic && !at_field_or_method) {
                break;
            }
            if (p.eat_if(Token::Dot)) {
                p.expect(Token::Ident);
                p.reduce(m, SyntaxKind::FieldAccess);
                continue;
            }
            auto binop = p.at(sets::binary_op) ? binop_from_token(p.current) : std::nullopt;
            if (binop) {
                auto op = *binop;
                auto prec = precedence(op);
                if (prec < min_prec) {
                    break;
                }
                if (assoc(op) == Assoc::Left) {
                    prec++;
                }
                p.eat();
                code_expr_prec(p, false, prec);
                p.reduce(m, SyntaxKind::Binary);
                continue;
            }
            break;
        };
    }

    static auto code_primary(Parser& p, bool atomic) -> void {
        // auto m = p.marker();
        switch (p.current) {
        case Token::Ident:
            p.eat();
            break;
            // Arrow => Closure
            // SyntaxKind::Underscore
        case Token::LBrace:
            code_block(p);
            break;
        case Token::LParen:
            expr_with_paren(p, atomic);
        case Token::Int:
        case Token::Float:
            p.eat();
            break;
        default:
            p.expected("expression");
            break;
        }
    }

    static auto code_block(Parser& p) -> void {
        constexpr TokenSet end{Token::RBrace, Token::RBracket, Token::RParen};
        auto m = p.marker();
        p.assert(Token::LBrace);
        code(p, [end](Parser& p) { return p.at(end); });
        p.expect_closing_delimiter(m, Token::RBrace);
        p.reduce(m, SyntaxKind::CodeBlock);
    }

    static auto expr_with_paren(Parser& p, bool) -> void {
        auto m = p.marker();
        p.assert(Token::LParen);
        code_expr(p);
        p.expect_closing_delimiter(m, Token::RBrace);
        p.reduce(m, SyntaxKind::Parenthesized);
    }
};

auto Parser::parse(std::string_view text) -> SyntaxNode {
    Parser p{text};
    auto m = p.marker();
    p.skip();
    Parser::Impl::code_exprs(p, [](Parser&) { return false; });
    p.reduce_all(m, SyntaxKind::CodeBlock);
    return p.finish().at(0);
}

void Parser::reduce_within(Marker from, Marker to, SyntaxKind kind) {
    auto to_{std::min(to.v, nodes.size())};
    auto first{nodes.begin() + to_};
    auto last{nodes.begin() + std::min(from.v, to_)};
    std::vector<SyntaxNode> children;
    children.reserve(first - last);
    std::ranges::move(last, first, std::back_inserter(children));
    nodes.erase(last, first);
    nodes.insert(last, SyntaxNode::inner(kind, std::move(children)));
}

void Parser::save() {
    auto text{current_text()};
    if (at(Token::Error)) {
        auto message = lexer.take_error().value();
        // get lex error
    } else {
        nodes.push_back(SyntaxNode::leaf(current, text));
    }
    if (!is_trivia(current)) {
        prev_end = current_end();
    }
}

void Parser::lex() {
    current_start = lexer.cursor();
    current = lexer.next_token();
    // process newline
}

} // namespace matoy::syntax
