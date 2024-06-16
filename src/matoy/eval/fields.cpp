#include "fields.hpp"
#include "matoy/foundations/matrix_op.hpp"
#include "matoy/utils/match.hpp"

namespace matoy::eval {

auto get_field(const Value& self, std::string_view field) -> diag::StrResult<Value> {
    return self.visit(utils::overloaded{
        [field](const Matrix& matrix) -> diag::StrResult<Value> {
            if (field == "T") {
                return matrix.transposed();
            }
            if (field == "I") {
                return ok_or_else(inverse(matrix), []() { return std::format("the matrix is not invertible"); });
            }
            return std::unexpected{std::format("type matrix does not contain field \"{}\"", field)};
        },
        [](const auto&) -> diag::StrResult<Value> { return std::unexpected{"cannot access fields on type"}; },
    });
}

} // namespace matoy::eval
