#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/value.hpp"

namespace matoy::syntax::ast {}

namespace matoy::eval {

namespace ast = syntax::ast;

using ValueResult = diag::HintedResult<Value>;

} // namespace matoy::eval
