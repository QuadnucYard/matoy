#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/value.hpp"

namespace matoy::eval {

using foundations::Value;
using ValueResult = diag::HintedResult<Value>;

} // namespace matoy::eval
