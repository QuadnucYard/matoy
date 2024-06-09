#include "matrix.hpp"
#include <cassert>

namespace matoy::foundations {

Matrix Matrix::transposed() const {
    auto res{Matrix::zeros(cols, rows)};
    for (size_t i{0}; i < rows; i++) {
        for (size_t j{0}; j < cols; j++) {
            res[j, i] = (*this)[i, j];
        }
    }
    return res;
}

Matrix Matrix::operator+() const {
    return *this;
}

Matrix Matrix::operator-() const {
    auto res{*this};
    for (size_t i{0}; i < data.size(); i++) {
        res.data[i] = -res.data[i];
    }
    return res;
}

Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
    assert(lhs.shape() == rhs.shape());
    auto res{lhs};
    res += rhs;
    return res;
}

Matrix operator+(const Matrix& lhs, Matrix::value_type rhs) {
    auto res{lhs};
    res += rhs;
    return res;
}

Matrix operator+(Matrix::value_type lhs, const Matrix& rhs) {
    auto res{rhs};
    res += lhs;
    return res;
}

Matrix operator-(const Matrix& lhs, const Matrix& rhs) {
    assert(lhs.shape() == rhs.shape());
    auto res{lhs};
    res -= rhs;
    return res;
}

Matrix operator-(const Matrix& lhs, Matrix::value_type rhs) {
    auto res{lhs};
    res -= rhs;
    return res;
}

Matrix operator-(Matrix::value_type lhs, const Matrix& rhs) {
    auto res{rhs};
    res -= lhs;
    return res;
}

Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
    assert(lhs.cols == rhs.rows);
    auto res{Matrix::zeros(lhs.rows, rhs.cols)};
    auto v{res.view()};
    auto v1{lhs.view()};
    auto v2{rhs.view()};
    for (size_t i{0}; i < lhs.rows; i++) {
        for (size_t k{0}; k < lhs.cols; k++) {
            for (size_t j{0}; j < rhs.cols; j++) {
                v[i, j] += v1[i, k] * v2[k, j];
            }
        }
    }
    return res;
}

Matrix operator*(const Matrix& lhs, Matrix::value_type rhs) {
    auto res{lhs};
    res *= rhs;
    return res;
}

Matrix operator*(Matrix::value_type lhs, const Matrix& rhs) {
    auto res{rhs};
    res *= lhs;
    return res;
}

Matrix operator/(const Matrix& lhs, Matrix::value_type rhs) {
    auto res{lhs};
    res /= rhs;
    return res;
}

Matrix& Matrix::operator+=(const Matrix& other) {
    assert(shape() == other.shape());
    for (size_t i{0}; i < data.size(); i++) {
        data[i] += other.data[i];
    }
    return *this;
}

Matrix& Matrix::operator+=(value_type value) {
    for (size_t i{0}; i < data.size(); i++) {
        data[i] += value;
    }
    return *this;
}

Matrix& Matrix::operator-=(const Matrix& other) {
    assert(shape() == other.shape());
    for (size_t i{0}; i < data.size(); i++) {
        data[i] -= other.data[i];
    }
    return *this;
}

Matrix& Matrix::operator-=(value_type value) {
    for (size_t i{0}; i < data.size(); i++) {
        data[i] -= value;
    }
    return *this;
}

Matrix& Matrix::operator*=(const Matrix& other) {
    return *this = *this * other;
}

Matrix& Matrix::operator*=(value_type value) {
    for (size_t i{0}; i < data.size(); i++) {
        data[i] *= value;
    }
    return *this;
}

Matrix& Matrix::operator/=(value_type value) {
    for (size_t i{0}; i < data.size(); i++) {
        data[i] /= value;
    }
    return *this;
}

} // namespace matoy::foundations
