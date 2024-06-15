#pragma once

#include "matrix.hpp"
#include <optional>

namespace matoy::foundations {

auto concat_h(const Matrix& a, const Matrix& b) -> Matrix;

auto concat_v(const Matrix& a, const Matrix& b) -> Matrix;

auto det(Matrix mat) -> Matrix::value_type;

auto inverse(const Matrix& mat) -> std::optional<Matrix>;

} // namespace matoy::foundations
