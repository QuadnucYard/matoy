#include "access.hpp"
#include "matoy/eval/vm.hpp"

namespace matoy::eval {

template <>
auto access(const ast::Ident& self, Vm& vm) -> diag::SourceResult<Value*> {
    return diag::to_source_error(vm.scopes.get_mut(self.get()), self.span());
}

template <>
auto access(const ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value*> {
    return access(self.expr(), vm);
}

template <>
auto access(const ast::FieldAccess&, Vm&) -> diag::SourceResult<Value*> {
    throw "unimplemented!";
}

template <>
auto access(const ast::FuncCall&, Vm&) -> diag::SourceResult<Value*> {
    throw "unimplemented!";
}

template <>
auto access(const ast::Expr& self, Vm& vm) -> diag::SourceResult<Value*> {
    return self.visit([&vm, &self](const auto& v) -> diag::SourceResult<Value*> {
        if constexpr (requires { access(v, vm); }) {
            return access(v, vm);
        } else {
            return diag::source_error(syntax::get_span(self), "cannot mutate a temporary value");
        }
    });
}

} // namespace matoy::eval
