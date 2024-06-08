#pragma once

#include "matoy/foundations/matrix.hpp"
#include <variant>

namespace matoy::foundations {

using Value = std::variant<int64_t, double, Matrix>;

}
