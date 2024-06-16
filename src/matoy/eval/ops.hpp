#pragma once

#include "fwd.hpp"
#include "matoy/diag.hpp"
#include <compare>

namespace matoy::eval {

auto pos(Value rhs) -> ValueResult;

auto neg(Value rhs) -> ValueResult;

auto not_(Value rhs) -> ValueResult;

auto add(Value lhs, Value rhs) -> ValueResult;

auto sub(Value lhs, Value rhs) -> ValueResult;

auto mul(Value lhs, Value rhs) -> ValueResult;

auto div(Value lhs, Value rhs) -> ValueResult;

auto and_(Value lhs, Value rhs) -> ValueResult;

auto or_(Value lhs, Value rhs) -> ValueResult;

auto equal(const Value& lhs, const Value& rhs) -> bool;

auto compare(const Value& lhs, const Value& rhs) -> diag::HintedResult<std::partial_ordering>;

auto eq(Value lhs, Value rhs) -> ValueResult;

auto neq(Value lhs, Value rhs) -> ValueResult;

auto lt(Value lhs, Value rhs) -> ValueResult;

auto leq(Value lhs, Value rhs) -> ValueResult;

auto gt(Value lhs, Value rhs) -> ValueResult;

auto geq(Value lhs, Value rhs) -> ValueResult;

auto aeq(Value lhs, Value rhs) -> ValueResult;

} // namespace matoy::eval
