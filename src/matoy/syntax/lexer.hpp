#pragma once

#include "scanner.hpp"
#include "token.hpp"
#include <cctype>
#include <optional>

namespace matoy::syntax {

class Lexer {

  public:
    Lexer(Scanner&& s) : s{std::move(s)} {}

    Token next();

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
