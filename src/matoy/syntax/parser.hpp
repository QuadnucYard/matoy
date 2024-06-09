#pragma once

#include "kind.hpp"
#include "lexer.hpp"
#include "node.hpp"
#include "token.hpp"
#include "token_set.hpp"
#include <cstdlib>
#include <string_view>
#include <vector>

namespace matoy::syntax {

struct Marker {
    size_t v;

    size_t operator*() const {
        return v;
    }
};

class Parser {
  public:
    Parser(std::string_view text, size_t offset = 0)
        : text{text}, lexer{text}, prev_end{offset}, current_start{offset}, current{lexer.next().v} {}

    static auto parse(std::string_view text) -> SyntaxNode;

  private:
    size_t current_end() const {
        return lexer.cursor();
    }

    Span current_span() const {
        return {current_start, current_end()};
    }

    std::string_view current_text() const {
        return text.substr(current_start, current_end() - current_start);
    }

    std::vector<SyntaxNode> finish() {
        return std::move(nodes);
    }

    bool at(Token token) const {
        return current == token;
    }

    bool at(TokenSet tokens) const {
        return tokens.contains(current);
    }

    bool directly_at(Token token) const {
        return current == token && prev_end == current_start;
    }

    bool end() const {
        return at(Token::End);
    }

    void eat() {
        save();
        lex();
        skip();
    }

    SyntaxNode& eat_and_get() {
        auto offset = nodes.size();
        eat();
        return nodes[offset];
    }

    bool eat_if(Token token) {
        if (at(token)) {
            eat();
            return true;
        }
        return false;
    }

    void assert_cur(Token token) {
        if (current != token) {
            std::abort();
        }
        eat();
    }

    Marker marker() const {
        return {nodes.size()};
    }

    // Get a marker after the last non-trivia node.
    Marker before_trivia() const {
        size_t i = nodes.size();
        if (prev_end != current_start) {
            while (i > 0 && is_trivia(nodes[i - 1].token())) {
                i--;
            }
        }
        return {i};
    }

    bool after_error() const {
        auto m = before_trivia();
        return *m > 0 && nodes[*m - 1].stores_error();
    }

    void reduce(Marker from, SyntaxKind kind) {
        reduce_within(from, before_trivia(), kind);
    }

    void reduce_all(Marker from, SyntaxKind kind) {
        reduce_within(from, Marker{nodes.size()}, kind);
    }

    void reduce_within(Marker from, Marker to, SyntaxKind kind);

    void skip() {
        while (is_trivia(current)) {
            save();
            lex();
        }
    }

    void save();

    void lex();

    /// Consume the given token or produce an error.
    bool expect(Token token);

    /// Produce an error that the given `thing` was expected.
    void expected(std::string_view thing);

    /// Produce an error that the given `thing` was expected at the position of the marker `m`.
    void expected_at(Marker m, std::string_view thing);

    /// Consume the next token (if any) and produce an error stating that it was unexpected.
    void unexpected();

    /// Consume the given closing delimiter or produce an error for the matching opening delimiter at `open`.
    void expect_closing_delimiter(Marker open, Token token);

    // Remove trailing errors with zero length.
    void trim_errors();

  private:
    struct Impl;

    std::string_view text;
    Lexer lexer;

    size_t prev_end;
    size_t current_start;
    Token current;
    std::vector<SyntaxNode> nodes;
};

} // namespace matoy::syntax
