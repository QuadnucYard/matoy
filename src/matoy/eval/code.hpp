#pragma once

#include "matoy/diag.hpp"
#include "matoy/eval/access.hpp"
#include "matoy/eval/ops.hpp"
#include "matoy/syntax/ast.hpp"
#include "vm.hpp"

namespace matoy::eval {

template <typename T>
auto eval(const T& self, Vm& vm) -> diag::SourceResult<Value>;

template <> auto eval(const syntax::ast::Expr& self, Vm& vm) -> diag::SourceResult<Value>;

auto apply_binary(const syntax::ast::Binary& binary, Vm& vm,
                  auto op(Value, Value)->ValueResult) -> diag::SourceResult<Value>;

auto apply_assignment(const syntax::ast::Binary& binary, Vm& vm,
                      auto op(Value, Value)->ValueResult) -> diag::SourceResult<Value>;

auto decl_assign(const syntax::ast::Binary& binary, Vm& vm) -> diag::SourceResult<Value>;

template <>
inline auto eval(const syntax::ast::Ident& self, Vm& vm) -> diag::SourceResult<Value> {
    return *vm.scopes.get(self.get()).value();
}

template <>
inline auto eval(const syntax::ast::Int& self, Vm&) -> diag::SourceResult<Value> {
    return self.get();
}

template <>
inline auto eval(const syntax::ast::Float& self, Vm&) -> diag::SourceResult<Value> {
    return self.get();
}

template <>
inline auto eval(const syntax::ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value> {
    return eval(self.expr(), vm);
}

template <>
inline auto eval(const syntax::ast::CodeBlock& self, Vm& vm) -> diag::SourceResult<Value> {
    Value output;
    for (auto&& expr : self.exprs()) {
        auto res = eval(expr, vm);
        if (!res)
            return res;
        output = *res;
    }
    return output;
}

template <>
inline auto eval(const syntax::ast::Unary& self, Vm& vm) -> diag::SourceResult<Value> {
    auto value = eval(self.expr(), vm);
    if (!value)
        return value;
    switch (self.op()) {
    case syntax::UnOp::Pos:
        return pos(*value).value();
    case syntax::UnOp::Neg:
        return neg(*value).value();
        break;
    }
}

template <>
inline auto eval(const syntax::ast::Binary& self, Vm& vm) -> diag::SourceResult<Value> {
    switch (self.op()) {
    case syntax::BinOp::Add:
        return apply_binary(self, vm, add);
    case syntax::BinOp::Sub:
        return apply_binary(self, vm, sub);
    case syntax::BinOp::Mul:
        return apply_binary(self, vm, mul);
    case syntax::BinOp::Div:
        return apply_binary(self, vm, div);
    case syntax::BinOp::Eq:
    case syntax::BinOp::Neq:
    case syntax::BinOp::Lt:
    case syntax::BinOp::Leq:
    case syntax::BinOp::Gt:
    case syntax::BinOp::Geq:
        throw "unimplemented!";
    case syntax::BinOp::Assign:
        return apply_assignment(self, vm, +[](Value, Value b) -> ValueResult { return b; });
    case syntax::BinOp::DeclAssign:
        return decl_assign(self, vm);
    case syntax::BinOp::AddAssign:
        return apply_assignment(self, vm, add);
    case syntax::BinOp::SubAssign:
        return apply_assignment(self, vm, sub);
    case syntax::BinOp::MulAssign:
        return apply_assignment(self, vm, mul);
    case syntax::BinOp::DivAssign:
        return apply_assignment(self, vm, div);
    }
}

template <>
inline auto eval(const syntax::ast::FieldAccess& self, Vm& vm) -> diag::SourceResult<Value> {
    return {};
}

template <>
inline auto eval(const syntax::ast::FuncCall& self, Vm& vm) -> diag::SourceResult<Value> {
    return {};
}

template <>
inline auto eval(const syntax::ast::Expr& self, Vm& vm) -> diag::SourceResult<Value> {
    auto v = self.visit([&vm](auto& e) -> diag::SourceResult<Value> { return eval(e, vm); });
    return v;
}

inline auto apply_binary(const syntax::ast::Binary& binary, Vm& vm,
                         auto op(Value, Value)->ValueResult) -> diag::SourceResult<Value> {
    auto lhs = eval(binary.lhs(), vm);
    if (!lhs)
        return lhs;
    // short-circuit
    auto rhs = eval(binary.rhs(), vm);
    if (!rhs)
        return rhs;
    return op(*lhs, *rhs).value();
}

inline auto apply_assignment(const syntax::ast::Binary& binary, Vm& vm,
                             auto op(Value, Value)->ValueResult) -> diag::SourceResult<Value> {
    auto rhs = eval(binary.rhs(), vm);
    auto loc = access(binary.lhs(), vm);
    auto res = op(**loc, *rhs);
    **loc = *res;
    return *res;
}

inline auto decl_assign(const syntax::ast::Binary& binary, Vm& vm) -> diag::SourceResult<Value> {
    auto ident = std::get<syntax::ast::Ident>(binary.lhs());
    auto rhs = eval(binary.rhs(), vm);
    vm.define(ident.get(), *rhs);
    return rhs;
}

} // namespace matoy::eval
