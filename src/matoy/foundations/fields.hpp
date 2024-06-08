#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/matrix.hpp"
#include "matoy/utils/match.hpp"
#include "value.hpp"
#include <string_view>

namespace matoy::foundations {

inline auto get_field(const Value& self, std::string_view field) -> diag::StrResult<Value> {
    return self.visit(utils::overloaded{
        [field](const Matrix& matrix) -> diag::StrResult<Value> {
            if (field == "T") {
                return matrix.transposed();
            } else {
                return {};
            }
        },
        [](const auto&) -> diag::StrResult<Value> { return {}; },
    });
}

} // namespace matoy::foundations
