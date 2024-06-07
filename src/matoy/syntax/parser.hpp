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

    bool eat_if(Token token) {
        if (at(token)) {
            eat();
            return true;
        }
        return false;
    }

    void assert(Token token) {
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

    bool expect(Token token) {
        if (this->at(token)) {
            eat();
            return true;
        } else if (token == Token::Ident && is_keyword(token)) {
            // self.trim_errors();
            // self.eat_and_get().expected(kind.name());
        } else {
            // self.balanced &= !kind.is_grouping();
            // self.expected(kind.name());
        }
        return false;
    }

    void expected(std::string_view) {}

    void expect_closing_delimiter(Marker, Token token) {
        if (!eat_if(token)) {
            // self.nodes[open.0].convert_to_error("unclosed delimiter");
        }
    }

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
