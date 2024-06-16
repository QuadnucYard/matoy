#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/value.hpp"
#include "matoy/syntax/ast.hpp"

namespace matoy::eval {

namespace ast = syntax::ast;

using ValueResult = diag::HintedResult<Value>;

class Vm;

template <typename T>
auto eval(const T& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Ident& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::None&, Vm&) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Int& self, Vm&) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Float& self, Vm&) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Bool& self, Vm&) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Matrix& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Code& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::CodeBlock& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Unary& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Binary& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::FieldAccess& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::FuncCall&, Vm&) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Expr& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::Conditional& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::WhileLoop&, Vm&) -> diag::SourceResult<Value>;

template <> auto eval(const ast::ForLoop& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::LoopBreak& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const ast::LoopContinue&, Vm&) -> diag::SourceResult<Value>;

template <> auto eval(const ast::FuncReturn& self, Vm& vm) -> diag::SourceResult<Value>;

} // namespace matoy::eval
