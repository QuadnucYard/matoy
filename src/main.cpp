#include "matoy/eval/eval.hpp"
#include "matoy/eval/scope.hpp"
#include "matoy/syntax/lexer.hpp"
#include "matoy/syntax/parser.hpp"
#include "matoy/utils/match.hpp"
#include <iomanip>
#include <magic_enum.hpp>
#include <print>
#include <sstream>
#include <string_view>

using namespace matoy;
using namespace matoy::syntax;
using namespace matoy::utils;

std::string escape_chars(std::string_view str) {
    std::ostringstream s;
    s << std::quoted(str);
    return s.str();
}

template <>
struct std::formatter<Token> : std::formatter<char> {
    auto format(const Token& token, format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", magic_enum::enum_name(token));
    }
};

template <>
struct std::formatter<SyntaxKind> : std::formatter<char> {
    auto format(const SyntaxKind& kind, format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", magic_enum::enum_name(kind));
    }
};

template <>
struct std::formatter<Span> : std::formatter<char> {
    auto format(const Span& span, format_context& ctx) const {
        return std::format_to(ctx.out(), "{}:{}", span.start, span.end);
    }
};

void print_tokens(std::string_view input) {
    Lexer lexer{input};
    for (auto t = lexer.next(); t.v != Token::End; t = lexer.next()) {
        std::println("{}[{}] `{}`", magic_enum::enum_name(t.v), t.span,
                     escape_chars(input.substr(t.span.start, t.span.end - t.span.start)));
    }
}

void print_node(const SyntaxNode& node, int depth = 0) {
    node.v.visit(overloaded{
        [depth](const LeafNode& leaf) {
            std::println("{:>{}}{} `{}` [{}]", "", depth * 2, leaf.token, leaf.text, leaf.span);
        },
        [depth](const InnerNode& inner) {
            std::println("{:>{}}{} [{}] (len: {}, desc: {})", "", depth * 2, inner.kind, inner.span, inner.len,
                         inner.descendants);
            for (auto& child : inner.children) {
                print_node(child, depth + 1);
            }
        },
    });
}

void test_lexer() {
    std::string_view input{"A = -3.876+ Aa - (bb * af / 13 + 5.765) "};
    print_tokens(input);
}

void print_syntax(std::string_view input) {
    auto node = Parser::parse(input);
    print_node(node);
}

void test_parser() {
    std::string_view input{R"(
    A := [1 + 6, 2 - (4 * 1);
            -31,        4.73]
    )"};
    print_tokens(input);
    print_syntax(input);
    return;
}

void test_eval() {
    std::string_view input{R"(
    A := [1 + 6, 2 - (4 * 1);
            -31,        4.73]
    B := [1, 2]
    C := [3, 4].T
    B * C
    )"};
    print_tokens(input);
    print_syntax(input);
    eval::Scope scope{};
    auto output = eval::eval_string(input, std::move(scope));
    std::println("done!");
    if (output) {
        std::println("output: {}", output.value());
    }
}

int main() {
    test_eval();
}
