#include "matoy/console/console.hpp"
#include "matoy/eval/eval.hpp"
#include "matoy/eval/scope.hpp"
#include "matoy/syntax/lexer.hpp"
#include "matoy/syntax/parser.hpp"
#include "matoy/utils/match.hpp"
#include <iomanip>
#include <print>
#include <sstream>
#include <string_view>

using namespace matoy;
using namespace matoy::console;
using namespace matoy::syntax;
using namespace matoy::utils;

std::string escape_chars(std::string_view str) {
    std::ostringstream s;
    s << std::quoted(str);
    return s.str();
}

void print_tokens(std::string_view input) {
    Lexer lexer{input};
    for (auto t = lexer.next(); t.v != Token::End; t = lexer.next()) {
        std::println("{}[{}] `{}`", t.v, t.span, escape_chars(input.substr(t.span.start, t.span.end - t.span.start)));
    }
}

void print_node(const SyntaxNode& node, int depth = 0) {
    node.v.visit(overloaded{
        [depth](const LeafNode& leaf) {
            std::println("{:>{}}{} `{}` [{}]", "", depth * 2, leaf.token, leaf.text, leaf.span);
        },
        [depth](const InnerNode& inner) {
            std::println("{:>{}}{} [{}] (len: {}, desc: {})", "", depth * 2, inner.kind, inner.span, inner.len(),
                         inner.descendants);
            for (auto& child : inner.children) {
                print_node(child, depth + 1);
            }
        },
        [depth](const ErrorNode& error) { std::println("{:>{}}ERROR `{}`", "", depth * 2, error.text); },
    });
}

void test_lexer() {
    std::string_view input{"A = -3.876+ Aa - (bb * af / 13 + 5.765) "};
    print_tokens(input);
}

void print_syntax(std::string_view input) {
    auto [node, _] = Parser::parse(input);
    print_node(node);
}

void test_parser() {
    std::string_view input{R"(
    {
        1.2
    }
    1 + 3
    { 3.4 }
    if 1.0 < 2.0 { 5.0 } else { 6.0 }
    if 1.0 > 2.0 {
        5.0
    } else {
        6.0
    }
    i := 0
    while i < 2 {
        i += 1
    }
    )"};
    print_tokens(input);
    print_syntax(input);
    return;
}

void test_eval() {
    std::string_view input{R"(
    i := 0
    s := 1
    while i < 5 {
        s *= 2
        i += 1
    }
    i
    s
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
    // test_parser();
    test_eval();
    Console console;
    console.start();
    console.loop();
}
