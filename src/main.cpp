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

void test_lexer() {
    std::string_view input{"A = -3.876+ Aa - (bb * af / 13 + 5.765) "};
    Lexer lexer{input};
    for (auto t = lexer.next(); t.v != Token::End; t = lexer.next()) {
        std::println("{}[{}:{}]", magic_enum::enum_name(t.v), t.span.start, t.span.end);
    }
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

void print_node(const SyntaxNode& node, int depth = 0) {
    node.v.visit(overloaded{
        [depth](const LeafNode& leaf) { std::println("{:>{}}{} `{}`", "", depth * 2, leaf.token, leaf.text); },
        [depth](const InnerNode& inner) {
            std::println("{:>{}}{} ({}, {})", "", depth * 2, inner.kind, inner.len, inner.descendants);
            for (auto& child : inner.children) {
                print_node(child, depth + 1);
            }
        },
    });
}

void test_parser() {
    // std::string_view input{"A = -3.876+ Aa - (bb * af / 13 + 5.765) "};
    std::string_view input{R"(
    A = 1.3
    A + 4
    )"};
    auto node = Parser::parse(input);
    print_node(node);
    // dbg(node);
    return;
}

void test_eval() {
    std::string_view input{R"(
    // A = 1.3
    // A = 1.3 + 7.3 - 7.4
    A = A + 4
    A
    )"};
    eval::Scope scope{};
    scope.define("A", 3.3);
    auto output = eval::eval_string(input, std::move(scope));
    std::println("done!");
    if (output) {
        std::println("output get double: {}", std::get<double>(output.value()));
    }
    // dbg(output);
}

int main() {
    // test_lexer();
    // test_parser();
    test_eval();
}
