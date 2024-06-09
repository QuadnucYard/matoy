#include "eval.hpp"
#include "matoy/diag.hpp"
#include "matoy/eval/code.hpp"
#include "matoy/eval/vm.hpp"
#include "matoy/syntax/ast.hpp"
#include "matoy/syntax/node.hpp"
#include "matoy/syntax/parser.hpp"
#include <ranges>
#include <string_view>

namespace matoy::eval {

auto eval_string(std::string_view str, Scope&& scope) -> diag::SourceResult<Value> {
    syntax::SyntaxNode node = syntax::Parser::parse(str);

    Vm vm;
    vm.scopes.scopes.push_back(std::move(scope));

    auto output = eval(node.cast<ast::CodeBlock>().value(), vm);

    return output;
}

auto eval_string(std::string_view str, Vm& vm) -> diag::SourceResult<Value> {
    syntax::SyntaxNode root = syntax::Parser::parse(str);

    auto errors = root.errors();
    if (!errors.empty()) {
        return std::unexpected{errors | std::views::as_rvalue |
                               std::views::transform(&diag::SyntaxError::operator diag::SourceDiagnostic) |
                               std::ranges::to<std::vector>()};
    }

    auto output = eval(root.cast<ast::CodeBlock>().value(), vm);

    return output;
}

} // namespace matoy::eval
