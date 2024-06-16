#include "eval.hpp"
#include "matoy/diag.hpp"
#include "matoy/eval/vm.hpp"
#include "matoy/syntax/ast.hpp"
#include "matoy/syntax/parser.hpp"
#include <print>
#include <ranges>
#include <string_view>

namespace matoy::eval {

auto eval_string(std::string_view str, Scope&& scope) -> diag::SourceResult<Value> {
    Vm vm;
    vm.scopes.scopes.push_back(std::move(scope));

    return eval_string(str, vm);
}

auto eval_string(std::string_view str, Vm& vm) -> diag::SourceResult<Value> {
    auto [root, has_inner_error] = syntax::Parser::parse(str);

    auto errors = root.errors();
    if (!errors.empty()) {
        return std::unexpected{errors | std::views::as_rvalue |
                               std::views::transform(&diag::SyntaxError::operator diag::SourceDiagnostic) |
                               std::ranges::to<std::vector>()};
    }

    auto output = eval(root.cast<ast::Code>().value(), vm);

    return output;
}

auto try_eval_string(std::string_view str, Vm& vm) -> std::optional<diag::SourceResult<Value>> {
    auto [root, has_inner_error] = syntax::Parser::parse(str);

    auto errors = root.errors();
    if (!errors.empty()) {
        if (!has_inner_error) {
            return std::nullopt;
        }
        return std::unexpected{errors | std::views::as_rvalue |
                               std::views::transform(&diag::SyntaxError::operator diag::SourceDiagnostic) |
                               std::ranges::to<std::vector>()};
    }

    auto output = eval(root.cast<ast::Code>().value(), vm);

    return output;
}

} // namespace matoy::eval
