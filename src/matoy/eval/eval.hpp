#pragma once

#include "fwd.hpp"
#include "matoy/diag.hpp"
#include <string_view>

namespace matoy::eval {

class Scope;

auto eval_string(std::string_view str, Scope&& scope) -> diag::SourceResult<Value>;

} // namespace matoy::eval
