#include "matoy/syntax/lexer.hpp"
#include "matoy/syntax/parser.hpp"
#include "matoy/utils/match.hpp"
#include <magic_enum.hpp>
#include <print>
#include <string_view>

using namespace matoy::syntax;
using namespace matoy::utils;

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
    std::string_view input{"A = -3.876+ Aa - (bb * af / 13 + 5.765) "};
    auto node = Parser::parse(input);
    print_node(node);
    return;
}

int main() {
    // test_lexer();
    test_parser();
}
