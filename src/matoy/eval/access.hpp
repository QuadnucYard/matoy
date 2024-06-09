#pragma once

#include "fwd.hpp"
#include "matoy/diag.hpp"
#include "matoy/eval/vm.hpp"
#include "matoy/syntax/ast.hpp"

namespace matoy::eval {

template <typename T>
auto access(const T& self, Vm& vm) -> diag::SourceResult<Value*> = delete;

template <> auto access(const ast::Ident& self, Vm& vm) -> diag::SourceResult<Value*>;

template <> auto access(const ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value*>;

template <> auto access(const ast::Expr& self, Vm& vm) -> diag::SourceResult<Value*>;

template <>
inline auto access(const ast::Ident& self, Vm& vm) -> diag::SourceResult<Value*> {
    return diag::to_source_error(vm.scopes.get_mut(self.get()), self.span());
}

template <>
inline auto access(const ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value*> {
    return access(self.expr(), vm);
}

template <>
inline auto access(const ast::FieldAccess&, Vm&) -> diag::SourceResult<Value*> {
    throw "unimplemented!";
}

template <>
inline auto access(const ast::FuncCall&, Vm&) -> diag::SourceResult<Value*> {
    throw "unimplemented!";
}

template <>
inline auto access(const ast::Expr& self, Vm& vm) -> diag::SourceResult<Value*> {
    return self.visit([&vm, &self](const auto& v) -> diag::SourceResult<Value*> {
        if constexpr (requires { access(v, vm); }) {
            return access(v, vm);
        } else {
            return diag::source_error(syntax::get_span(self), "cannot mutate a temporary value");
        }
    });
}

} // namespace matoy::eval
