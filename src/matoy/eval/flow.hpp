#pragma once

#include "fwd.hpp"
#include "matoy/eval/vm.hpp"
#include "matoy/syntax/ast.hpp"

namespace matoy::eval {

template <>
inline auto eval(const ast::Conditional&, Vm&) -> diag::SourceResult<Value> {
    return {};
}

template <>
inline auto eval(const ast::WhileLoop&, Vm&) -> diag::SourceResult<Value> {
    return {};
}

template <>
inline auto eval(const ast::ForLoop&, Vm&) -> diag::SourceResult<Value> {
    return {};
}

template <>
inline auto eval(const ast::LoopBreak&, Vm&) -> diag::SourceResult<Value> {
    return {};
}

template <>
inline auto eval(const ast::LoopContinue&, Vm&) -> diag::SourceResult<Value> {
    return {};
}

template <>
inline auto eval(const ast::FuncReturn&, Vm&) -> diag::SourceResult<Value> {
    return {};
}

} // namespace matoy::eval
