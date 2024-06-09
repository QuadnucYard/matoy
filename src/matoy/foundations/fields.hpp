#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/matrix.hpp"
#include "matoy/utils/match.hpp"
#include "value.hpp"
#include <format>
#include <string_view>

namespace matoy::foundations {

inline auto get_field(const Value& self, std::string_view field) -> diag::StrResult<Value> {
    return self.visit(utils::overloaded{
        [field](const Matrix& matrix) -> diag::StrResult<Value> {
            if (field == "T") {
                return matrix.transposed();
            } else {
                return std::unexpected{std::format("type matrix does not contain field \"{}\"", field)};
            }
        },
        [](const auto&) -> diag::StrResult<Value> { return std::unexpected{"cannot access fields on type"}; },
    });
}

} // namespace matoy::foundations

namespace matoy {
using foundations::get_field;
}
