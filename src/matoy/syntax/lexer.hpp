#pragma once

#include "scanner.hpp"
#include "span.hpp"
#include "token.hpp"
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace matoy::syntax {

class Lexer {

  public:
    Lexer(std::string_view text) : s{text} {}

    size_t cursor() const {
        return this->s.cursor();
    }

    std::optional<std::string> take_error() {
        return std::move(err);
    }

    Spanned<Token> next();

    Token next_token();

  private:
    Token whitespace();

    Token line_comment();

    Token block_comment();

    Token code(size_t start, char c);

    Token ident(size_t start);

    Token number(size_t start, char c);

    Token error(std::string message);

  private:
    Scanner s;

    std::optional<std::string> err;
};

} // namespace matoy::syntax
