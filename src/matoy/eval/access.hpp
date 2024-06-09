#pragma once

#include "fwd.hpp"
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
    return vm.scopes.get_mut(self.get()).value();
}

template <>
inline auto access(const ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value*> {
    return access(self.expr(), vm);
}

template <>
inline auto access(const ast::FieldAccess& self, Vm& vm) -> diag::SourceResult<Value*> {
    throw "unimplemented!";
}

template <>
inline auto access(const ast::FuncCall& self, Vm& vm) -> diag::SourceResult<Value*> {
    throw "unimplemented!";
}

template <>
inline auto access(const ast::Expr& self, Vm& vm) -> diag::SourceResult<Value*> {
    return self.visit([&vm](const auto& v) -> diag::SourceResult<Value*> {
        if constexpr (requires { access(v, vm); }) {
            return access(v, vm);
        } else {
            return std::unexpected{std::vector<diag::SourceDiagnostic>{}};
        }
    });
}

} // namespace matoy::eval
