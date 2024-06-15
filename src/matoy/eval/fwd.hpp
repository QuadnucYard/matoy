#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/value.hpp"

namespace matoy::syntax::ast {}

namespace matoy::eval {

namespace ast = syntax::ast;

using ValueResult = diag::HintedResult<Value>;

class Vm;

template <typename T>
auto eval(const T& self, Vm& vm) -> diag::SourceResult<Value> = delete;

} // namespace matoy::eval
