#include "parser.hpp"
#include "matoy/syntax/kind.hpp"
#include "matoy/syntax/op.hpp"
#include "matoy/syntax/token.hpp"
#include "matoy/utils/slice.hpp"
#include "token_set.hpp"
#include <algorithm>
#include <concepts>

namespace matoy::syntax {

using namespace utils;

struct Parser::Impl {

    static auto code(Parser& p, std::predicate<Parser&> auto stop) -> void {
        auto m = p.marker();
        code_exprs(p, stop);
        p.reduce(m, SyntaxKind::Code);
    }

    static auto code_exprs(Parser& p, std::predicate<Parser&> auto stop) -> void {
        while (!p.end() && !stop(p)) {
            if (p.at(sets::expr)) {
                code_expr(p);
                if (!p.end() && !stop(p) && !p.eat_if(Token::Semicolon)) {
                    // TODO handle statement deliminator
                    // p.expected("semicolon or line break");
                }
            } else if (!p.end()) {
                p.unexpected();
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
                args(p);
                p.reduce(m, SyntaxKind::FuncCall);
                continue;
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
        case Token::Ident:    p.eat(); break;
        case Token::LBrace:   code_block(p); break;
        case Token::LParen:   expr_with_paren(p, atomic); break;
        case Token::LBracket: matrix(p); break;

        case Token::If:       conditional(p); break;
        case Token::While:    while_loop(p); break;
        case Token::For:      for_loop(p); break;
        case Token::Break:    break_stmt(p); break;
        case Token::Continue: continue_stmt(p); break;
        case Token::Return:   return_stmt(p); break;

        case Token::None:
        case Token::Int:
        case Token::Float:
        case Token::Bool:
        case Token::Str:   p.eat(); break;

        default: p.expected("expression"); break;
        }
    }

    static auto code_block(Parser& p) -> void {
        constexpr TokenSet end{Token::RBrace, Token::RBracket, Token::RParen};
        auto m = p.marker();
        p.assert_cur(Token::LBrace);
        code(p, [end](Parser& p) { return p.at(end); });
        p.expect_closing_delimiter(m, Token::RBrace);
        p.reduce(m, SyntaxKind::CodeBlock);
    }

    static auto expr_with_paren(Parser& p, bool) -> void {
        auto m = p.marker();
        p.assert_cur(Token::LParen);
        code_expr(p);
        p.expect_closing_delimiter(m, Token::RParen);
        p.reduce(m, SyntaxKind::Parenthesized);
    }

    static auto args(Parser& p) -> void {
        if (!p.at(Token::LParen)) {
            p.expected("argument list");
        }

        auto m = p.marker();

        // Parses a normal positional argument or an argument name.
        p.assert_cur(Token::LParen);

        while (!is_terminator(p.current)) {
            if (!p.at(sets::expr)) {
                p.unexpected();
                continue;
            }

            code_expr(p);

            if (!is_terminator(p.current)) {
                p.expect(Token::Comma);
            }
        }

        p.expect_closing_delimiter(m, Token::RParen);

        p.reduce(m, SyntaxKind::Args);
    }

    /**
     * @brief Parse a matrix: `[1, 2; 3, 4]`
     */
    static auto matrix(Parser& p) -> void {
        auto m = p.marker();
        auto m1 = p.marker();

        p.assert_cur(Token::LBracket);

        size_t rows{}, cols{}, cur_col{};

        while (!is_terminator(p.current)) {
            code_expr(p);
            switch (p.current) {
            case Token::Comma:
                p.eat();
                cur_col++;
                if (rows == 0) {
                    cols++;
                }
                break;
            case Token::Semicolon:
                p.eat();
                cur_col++;
                if (rows++ == 0) {
                    cols = cur_col;
                } else if (cur_col != cols) {
                    // check shape
                    p.expected("the same column size");
                }
                // finishes a row
                p.reduce(m1, SyntaxKind::MatrixRow);
                m1 = p.marker();
                cur_col = 0;
                break;
            case Token::RBracket:
                rows++;
                p.reduce(m1, SyntaxKind::MatrixRow);
                m1 = p.marker();
                break;
            default: p.unexpected(); break;
            }
        }
        p.expect_closing_delimiter(m, Token::RBracket);
        p.reduce(m, SyntaxKind::Matrix);
    }

    /// Parses an if-else conditional: `if x { y } else { z }`.
    static auto conditional(Parser& p) -> void {
        auto m = p.marker();
        p.assert_cur(Token::If);
        code_expr(p);
        code_block(p);
        if (p.eat_if(Token::Else)) {
            if (p.at(Token::If)) {
                conditional(p);
            } else {
                code_block(p);
            }
        }
        p.reduce(m, SyntaxKind::Conditional);
    }

    /// Parses a while loop: `while x { y }`.
    static auto while_loop(Parser& p) -> void {
        auto m = p.marker();
        p.assert_cur(Token::While);
        code_expr(p);
        code_block(p);
        p.reduce(m, SyntaxKind::WhileLoop);
    }

    /// Parses a for loop: `for x in y { z }`.
    static auto for_loop(Parser&) -> void {
        throw "unimplemented";
    }

    static auto break_stmt(Parser& p) -> void {
        auto m = p.marker();
        p.assert_cur(Token::Break);
        p.reduce(m, SyntaxKind::LoopBreak);
    }

    static auto continue_stmt(Parser& p) -> void {
        auto m = p.marker();
        p.assert_cur(Token::Continue);
        p.reduce(m, SyntaxKind::LoopContinue);
    }

    static auto return_stmt(Parser& p) -> void {
        auto m = p.marker();
        p.assert_cur(Token::Return);
        p.reduce(m, SyntaxKind::FuncReturn);
    }
};

auto Parser::parse(std::string_view text) -> std::pair<SyntaxNode, bool> {
    Parser p{text};
    auto m = p.marker();
    p.skip();
    Parser::Impl::code_exprs(p, [](Parser&) { return false; });
    p.reduce_all(m, SyntaxKind::Code);
    return {std::move(p.finish().at(0)), p.has_inner_errors};
}

void Parser::reduce_within(Marker from, Marker to, SyntaxKind kind) {
    auto to_{std::min(*to, nodes.size())};
    auto from_{std::min(*from, to_)};
    auto first{nodes.begin() + from_};
    auto last{nodes.begin() + to_};

    Span span{};
    for (auto it = first; it != last; ++it) {
        if (!it->is_error()) {
            auto span2 = it->span();
            if (span.end == 0) {
                span.start = span2.start;
            }
            span.end = span2.end;
        }
    }
    auto text = slice(this->text, span.start, span.end);

    std::vector<SyntaxNode> children;
    children.reserve(last - first);
    std::ranges::move(first, last, std::back_inserter(children));
    nodes.erase(first, last);
    nodes.insert(first, SyntaxNode::inner(kind, text, std::move(children)));
}

void Parser::save() {
    auto text{current_text()};
    auto span = current_span();
    if (at(Token::Error)) {
        auto message = lexer.take_error().value();
        nodes.push_back(SyntaxNode::error(text, message, span));
    } else {
        nodes.push_back(SyntaxNode::leaf(current, text, span));
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

bool Parser::expect(Token token) {
    if (this->at(token)) {
        eat();
        return true;
    }
    if (token == Token::Ident && is_keyword(token)) {
        trim_errors();
        eat_and_get().expected_token(get_name(token));
    } else {
        // self.balanced &= !kind.is_grouping();
        expected(get_name(token));
    }
    return false;
}

void Parser::expected(std::string_view thing) {
    if (!after_error()) {
        expected_at(before_trivia(), thing);
    }
}

void Parser::expected_at(Marker m, std::string_view thing) {
    nodes.insert(nodes.begin() + *m, SyntaxNode::error("", std::format("expected {}", thing), current_span()));
    if (!end()) {
        has_inner_errors = true;
    }
}

void Parser::unexpected() {
    trim_errors();
    // balanced
    eat_and_get().unexpected();
}

void Parser::expect_closing_delimiter(Marker open, Token token) {
    if (!eat_if(token)) {
        nodes[*open].convert_to_error("unclosed delimiter");
    }
}

void Parser::trim_errors() {
    auto end = *before_trivia();
    auto start = end;
    while (start > 0 && nodes[start - 1].empty() && nodes[start - 1].stores_error()) {
        start--;
    }
    nodes.erase(nodes.begin() + start, nodes.begin() + end);
}

} // namespace matoy::syntax
