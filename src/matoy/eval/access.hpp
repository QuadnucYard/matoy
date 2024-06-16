#pragma once

#include "fwd.hpp"
#include "matoy/diag.hpp"

namespace matoy::eval {

class Vm;

template <typename T>
auto access(const T& self, Vm& vm) -> diag::SourceResult<Value*> = delete;

template <> auto access(const ast::Ident& self, Vm& vm) -> diag::SourceResult<Value*>;

template <> auto access(const ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value*>;

template <> auto access(const ast::FieldAccess&, Vm&) -> diag::SourceResult<Value*>;

template <> auto access(const ast::FuncCall&, Vm&) -> diag::SourceResult<Value*>;

template <> auto access(const ast::Expr& self, Vm& vm) -> diag::SourceResult<Value*>;

} // namespace matoy::eval
