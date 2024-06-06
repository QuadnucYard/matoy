#include "matoy/syntax/lexer.hpp"
#include <magic_enum.hpp>
#include <string_view>

using namespace std;

int main(int argc, char** argv) {
    using namespace matoy::syntax;

    std::string_view input{"A = -3.876+ Aa - (bb * af / 13 + 5.765)"};
    matoy::syntax::Lexer lexer{matoy::syntax::Scanner{input}};
    for (auto t = lexer.next(); t != Token::End; t = lexer.next()) {
        std::println("{}", magic_enum::enum_name(t));
    }
    return 0;
}
