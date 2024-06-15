#include "matrix_op.hpp"
#include <cassert>

namespace matoy::foundations {

static constexpr auto EPS = std::numeric_limits<Matrix::value_type>::epsilon();

auto concat_h(const Matrix& a, const Matrix& b) -> Matrix {
    assert(a.rows() == b.rows());

    const size_t n{a.rows()}, m1{a.cols()}, m2{b.cols()}, m{m1 + m2};
    Matrix res = Matrix::zeros(n, m);

    for (size_t i{}; i < n; i++) {
        std::ranges::copy(a.data() + i * m1, a.data() + (i + 1) * m1, res.data() + i * m);
        std::ranges::copy(b.data() + i * m2, b.data() + (i + 1) * m2, res.data() + i * m + m1);
    }

    return res;
}

auto concat_v(const Matrix& a, const Matrix& b) -> Matrix {
    assert(a.cols() == b.cols());

    const size_t m{a.cols()}, n1{a.rows()}, n2{b.rows()}, n{n1 + n2};
    Matrix res = Matrix::zeros(n, m);

    std::ranges::copy(a.data(), a.data() + a.size(), res.data());
    std::ranges::copy(b.data(), b.data() + b.size(), res.data() + n1 * m);

    return res;
}

auto det(Matrix mat) -> Matrix::value_type {
    assert(mat.is_square());

    size_t n = mat.rows();
    Matrix::value_type res{1.0};

    for (size_t i{}; i < n; i++) {
        // find the row with the greatest first element
        size_t k{i};
        for (size_t j{i}; j < n; j++) {
            if (std::abs(mat[j, i]) > std::abs(mat[k, i])) {
                k = j;
            }
        }
        if (std::abs(mat[k, i]) < EPS) {
            return 0.0;
        }
        if (k != i) {
            mat.swap_row(i, k);
            res = -res;
        }
        res *= mat[i, i];
        mat.multiply_row(i, 1.0 / mat[i, i]);
        for (size_t j{i + 1}; j < n; j++) {
            if (std::abs(mat[j, i]) > EPS) {
                mat.add_row_multiple(j, i, -mat[j, i]);
            }
        }
    }

    return res;
}

auto inverse(const Matrix& mat) -> std::optional<Matrix> {
    assert(mat.is_square());

    const size_t n{mat.rows()};
    Matrix a{concat_h(mat, Matrix::identity(n))};

    for (size_t i{}; i < n; i++) {
        // find the row with the greatest first element
        size_t k{i};
        for (size_t j{i}; j < n; j++) {
            if (std::abs(a[j, i]) > std::abs(a[k, i])) {
                k = j;
            }
        }
        if (std::abs(a[k, i]) < EPS) {
            return std::nullopt;
        }
        if (k != i) {
            a.swap_row(i, k);
        }
        a.multiply_row(i, 1.0 / a[i, i]);
        for (size_t j{i + 1}; j < n; j++) {
            if (std::abs(a[j, i]) > EPS) {
                a.add_row_multiple(j, i, -a[j, i]);
            }
        }
    }
    for (size_t i{n - 1}; ~i; i--) {
        for (size_t k{0}; k < i; k++) {
            a.add_row_multiple(k, i, -a[k, i]);
        }
    }

    // get the left part
    Matrix res = Matrix::zeros(n, n);
    for (size_t i{}; i < n; i++) {
        std::copy(a.data() + i * (n * 2) + n, a.data() + (i + 1) * (n * 2), res.data() + i * n);
    }
    return res;
}

} // namespace matoy::foundations
