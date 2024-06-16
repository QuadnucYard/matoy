#pragma once

#include "matoy/diag.hpp"
#include "matoy/eval/access.hpp"
#include "matoy/eval/fields.hpp"
#include "matoy/eval/fwd.hpp"
#include "matoy/eval/ops.hpp"
#include "matoy/foundations/matrix.hpp"
#include "matoy/foundations/value.hpp"
#include "matoy/syntax/ast.hpp"
#include "matoy/syntax/op.hpp"
#include "vm.hpp"

namespace matoy::eval {

auto apply_unary(const ast::Unary& unary, Vm& vm, auto op(Value)->ValueResult) -> diag::SourceResult<Value>;

auto apply_binary(const ast::Binary& binary, Vm& vm, auto op(Value, Value)->ValueResult) -> diag::SourceResult<Value>;

auto apply_assignment(const ast::Binary& binary, Vm& vm,
                      auto op(Value, Value)->ValueResult) -> diag::SourceResult<Value>;

auto decl_assign(const ast::Binary& binary, Vm& vm) -> diag::SourceResult<Value>;

template <>
inline auto eval(const ast::Ident& self, Vm& vm) -> diag::SourceResult<Value> {
    return diag::to_source_error(clone(vm.scopes.get(self.get())), self.span());
}

template <>
inline auto eval(const ast::None&, Vm&) -> diag::SourceResult<Value> {
    return values::none_t{};
}

template <>
inline auto eval(const ast::Int& self, Vm&) -> diag::SourceResult<Value> {
    return self.get();
}

template <>
inline auto eval(const ast::Float& self, Vm&) -> diag::SourceResult<Value> {
    return self.get();
}

template <>
inline auto eval(const ast::Bool& self, Vm&) -> diag::SourceResult<Value> {
    return self.get();
}

template <>
inline auto eval(const ast::Matrix& self, Vm& vm) -> diag::SourceResult<Value> {
    auto [rows, cols] = self.shape();
    std::vector<double> elems;
    for (auto&& item : self.items()) {
        auto val = eval(item, vm);
        if (!val)
            return val;
        if (auto p = std::get_if<int64_t>(&*val)) {
            elems.push_back(*p);
        } else if (auto p = std::get_if<double>(&*val)) {
            elems.push_back(*p);
        } else {
            return diag::source_error(get_span(item), "the item can't fit into a matrix");
        }
    }
    return Matrix(rows, cols, elems);
}

template <>
inline auto eval(const ast::Parenthesized& self, Vm& vm) -> diag::SourceResult<Value> {
    return eval(self.expr(), vm);
}

template <>
inline auto eval(const ast::Code& self, Vm& vm) -> diag::SourceResult<Value> {
    Value output = values::none_t{};
    for (auto&& expr : self.exprs()) {
        auto res = eval(expr, vm);
        if (!res)
            return res;
        output = std::move(*res);
    }
    return output;
}

template <>
inline auto eval(const ast::CodeBlock& self, Vm& vm) -> diag::SourceResult<Value> {
    vm.scopes.enter();
    auto output = eval(self.body(), vm);
    vm.scopes.exit();
    return output;
}

template <>
inline auto eval(const ast::Unary& self, Vm& vm) -> diag::SourceResult<Value> {
    switch (self.op()) {
    case syntax::UnOp::Pos: return apply_unary(self, vm, pos);
    case syntax::UnOp::Neg: return apply_unary(self, vm, neg);
    case syntax::UnOp::Not: return apply_unary(self, vm, not_);
    }
}

template <>
inline auto eval(const ast::Binary& self, Vm& vm) -> diag::SourceResult<Value> {
    switch (self.op()) {
    case syntax::BinOp::Add: return apply_binary(self, vm, add);
    case syntax::BinOp::Sub: return apply_binary(self, vm, sub);
    case syntax::BinOp::Mul: return apply_binary(self, vm, mul);
    case syntax::BinOp::Div: return apply_binary(self, vm, div);

    case syntax::BinOp::Eq:  return apply_binary(self, vm, eq);
    case syntax::BinOp::Neq: return apply_binary(self, vm, neq);
    case syntax::BinOp::Lt:  return apply_binary(self, vm, lt);
    case syntax::BinOp::Leq: return apply_binary(self, vm, leq);
    case syntax::BinOp::Gt:  return apply_binary(self, vm, gt);
    case syntax::BinOp::Geq: return apply_binary(self, vm, geq);

    case syntax::BinOp::Approx: return apply_binary(self, vm, aeq);

    case syntax::BinOp::And: return apply_binary(self, vm, and_);
    case syntax::BinOp::Or:  return apply_binary(self, vm, or_);

    case syntax::BinOp::Assign:     return apply_assignment(self, vm, +[](Value, Value b) -> ValueResult { return b; });
    case syntax::BinOp::DeclAssign: return decl_assign(self, vm);
    case syntax::BinOp::AddAssign:  return apply_assignment(self, vm, add);
    case syntax::BinOp::SubAssign:  return apply_assignment(self, vm, sub);
    case syntax::BinOp::MulAssign:  return apply_assignment(self, vm, mul);
    case syntax::BinOp::DivAssign:  return apply_assignment(self, vm, div);
    }
}

template <>
inline auto eval(const ast::FieldAccess& self, Vm& vm) -> diag::SourceResult<Value> {
    auto value = eval(self.target(), vm);
    if (!value)
        return value;
    auto field = self.field();
    return diag::to_source_error(get_field(*value, field.get()), self.span());
}

template <>
inline auto eval(const ast::FuncCall&, Vm&) -> diag::SourceResult<Value> {
    return {};
}

template <>
inline auto eval(const ast::Expr& self, Vm& vm) -> diag::SourceResult<Value> {
    return self.visit([&vm](auto& e) { return eval(e, vm); });
}

inline auto apply_unary(const ast::Unary& unary, Vm& vm, auto op(Value)->ValueResult) -> diag::SourceResult<Value> {
    auto value = eval(unary.expr(), vm);
    if (!value)
        return value;

    return diag::to_source_error(op(std::move(*value)), unary.span());
}

inline auto apply_binary(const ast::Binary& binary, Vm& vm,
                         auto op(Value, Value)->ValueResult) -> diag::SourceResult<Value> {
    auto lhs = eval(binary.lhs(), vm);
    if (!lhs)
        return lhs;

    // Short-circuit boolean operations.
    if ((binary.op() == syntax::BinOp::And && *lhs == Value{false}) ||
        (binary.op() == syntax::BinOp::Or && *lhs == Value{true})) {
        return lhs;
    }

    auto rhs = eval(binary.rhs(), vm);
    if (!rhs)
        return rhs;

    return diag::to_source_error(op(std::move(*lhs), std::move(*rhs)), binary.span());
}

inline auto apply_assignment(const ast::Binary& binary, Vm& vm,
                             auto op(Value, Value)->ValueResult) -> diag::SourceResult<Value> {
    auto rhs = eval(binary.rhs(), vm);
    if (!rhs)
        return rhs;

    auto loc = access(binary.lhs(), vm);
    if (!loc)
        return clone(std::move(loc));

    auto res = op(**loc, *rhs);
    if (!res)
        return diag::to_source_error(std::move(res), binary.span());

    **loc = *res;

    return *res;
}

inline auto decl_assign(const ast::Binary& binary, Vm& vm) -> diag::SourceResult<Value> {
    auto ident = std::get<ast::Ident>(binary.lhs());

    auto rhs = eval(binary.rhs(), vm);
    if (!rhs)
        return rhs;

    if (access(binary.lhs(), vm)) {
        return diag::source_error(ident.span(), std::format("the variable \"{}\" already exists", ident.get()));
    }

    vm.define(ident.get(), Value{*rhs});

    return rhs;
}

} // namespace matoy::eval
