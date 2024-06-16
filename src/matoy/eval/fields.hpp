#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/value.hpp"
#include <string_view>

namespace matoy::eval {

auto get_field(const Value& self, std::string_view field) -> diag::StrResult<Value>;

} // namespace matoy::eval
