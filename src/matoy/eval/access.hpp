#pragma once

#include "fwd.hpp"
#include "matoy/eval/vm.hpp"
#include "matoy/syntax/ast.hpp"

namespace matoy::eval {

template <typename T>
auto access(const T& self, Vm& vm) -> diag::SourceResult<Value*>;

template <> auto access(const syntax::ast::Ident& self, Vm& vm) -> diag::SourceResult<Value*>;

template <> auto access(const syntax::ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value*>;

template <> auto access(const syntax::ast::Expr& self, Vm& vm) -> diag::SourceResult<Value*>;

template <>
inline auto access(const syntax::ast::Ident& self, Vm& vm) -> diag::SourceResult<Value*> {
    return vm.scopes.get_mut(self.get()).value();
}

template <>
inline auto access(const syntax::ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value*> {
    return access(self.expr(), vm);
}

template <>
inline auto access(const syntax::ast::FieldAccess& self, Vm& vm) -> diag::SourceResult<Value*> {
    throw "unimplemented!";
}

template <>
inline auto access(const syntax::ast::FuncCall& self, Vm& vm) -> diag::SourceResult<Value*> {
    throw "unimplemented!";
}

template <>
inline auto access(const syntax::ast::Expr& self, Vm& vm) -> diag::SourceResult<Value*> {
    return self.visit(utils::overloaded{
        [&vm](const syntax::ast::Ident& v) { return access(v, vm); },
        [&vm](const syntax::ast::Parenthesized& v) { return access(v, vm); },
        [&vm](const syntax::ast::FieldAccess& v) { return access(v, vm); },
        [&vm](const syntax::ast::FuncCall& v) { return access(v, vm); },
        [](const auto&) -> diag::SourceResult<Value*> {
            return std::unexpected{std::vector<diag::SourceDiagnostic>{}};
        },
    });
    // if (std::holds_alternative<syntax::ast::Ident>(self)) {
    //     return access(std::get<syntax::ast::Ident>(self), vm).value();
    // }
    // if (std::holds_alternative<syntax::ast::Parenthesized>(self)) {
    //     return access(std::get<syntax::ast::Parenthesized>(self), vm).value();
    // }
    // if (std::holds_alternative<syntax::ast::FieldAccess>(self)) {
    //     return access(std::get<syntax::ast::FieldAccess>(self), vm).value();
    // }
    // if (std::holds_alternative<syntax::ast::FuncCall>(self)) {
    //     return access(std::get<syntax::ast::FuncCall>(self), vm).value();
    // }
    // throw "";
}

} // namespace matoy::eval
