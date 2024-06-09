#pragma once

#include "fwd.hpp"
#include "matoy/diag.hpp"
#include <string_view>

namespace matoy::eval {

class Scope;
class Vm;

auto eval_string(std::string_view str, Scope&& scope) -> diag::SourceResult<Value>;

auto eval_string(std::string_view str, Vm& vm) -> diag::SourceResult<Value>;

} // namespace matoy::eval
