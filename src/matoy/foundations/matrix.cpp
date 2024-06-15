#include "matrix.hpp"
#include <cassert>

namespace matoy::foundations {

Matrix::Matrix(std::initializer_list<std::initializer_list<Matrix::value_type>> l)
    : rows_{l.size()}, cols_{l.begin()->size()} {
    assert(l.size() > 0);

    data_.resize(rows_ * cols_);
    for (size_t i = 0; auto& l1 : l) {
        assert(l1.size() == cols_);
        std::ranges::copy(l1, data_.begin() + i * cols_);
        i++;
    }
}

Matrix Matrix::zeros(size_t rows, size_t cols, const Matrix::value_type& fill_value) {
    Matrix res;
    res.rows_ = rows;
    res.cols_ = cols;
    res.data_.resize(rows * cols, fill_value);
    return res;
}

Matrix Matrix::identity(size_t n) {
    Matrix res = Matrix::zeros(n, n);
    for (size_t i = 0; i < n; i++) {
        res[i, i] = 1.0;
    }
    return res;
}

Matrix Matrix::transposed() const {
    auto res{Matrix::zeros(cols_, rows_)};
    for (size_t i{0}; i < rows_; i++) {
        for (size_t j{0}; j < cols_; j++) {
            res[j, i] = (*this)[i, j];
        }
    }
    return res;
}

void Matrix::swap_row(size_t r1, size_t r2) {
    for (size_t j = 0; j < cols_; j++) {
        std::swap((*this)[r1, j], (*this)[r2, j]);
    }
}

void Matrix::multiply_row(size_t r, const Matrix::value_type& x) {
    for (size_t j = 0; j < cols_; j++) {
        (*this)[r, j] *= x;
    }
}

void Matrix::add_row_multiple(size_t r1, size_t r2, const Matrix::value_type& x) {
    for (size_t j = 0; j < cols_; j++) {
        (*this)[r1, j] += (*this)[r2, j] * x;
    }
}

Matrix Matrix::operator+() const {
    return *this;
}

Matrix Matrix::operator-() const {
    auto res{*this};
    for (size_t i{0}; i < data_.size(); i++) {
        res.data_[i] = -res.data_[i];
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
    assert(lhs.cols_ == rhs.rows_);
    auto res{Matrix::zeros(lhs.rows_, rhs.cols_)};
    auto v{res.view()};
    auto v1{lhs.view()};
    auto v2{rhs.view()};
    for (size_t i{0}; i < lhs.rows_; i++) {
        for (size_t k{0}; k < lhs.cols_; k++) {
            for (size_t j{0}; j < rhs.cols_; j++) {
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

bool operator==(const Matrix& lhs, const Matrix& rhs) {
    return lhs.shape() == rhs.shape() && lhs.data_ == rhs.data_;
}

Matrix& Matrix::operator+=(const Matrix& other) {
    assert(shape() == other.shape());
    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] += other.data_[i];
    }
    return *this;
}

Matrix& Matrix::operator+=(value_type value) {
    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] += value;
    }
    return *this;
}

Matrix& Matrix::operator-=(const Matrix& other) {
    assert(shape() == other.shape());
    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] -= other.data_[i];
    }
    return *this;
}

Matrix& Matrix::operator-=(value_type value) {
    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] -= value;
    }
    return *this;
}

Matrix& Matrix::operator*=(const Matrix& other) {
    return *this = *this * other;
}

Matrix& Matrix::operator*=(value_type value) {
    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] *= value;
    }
    return *this;
}

Matrix& Matrix::operator/=(value_type value) {
    for (size_t i{0}; i < data_.size(); i++) {
        data_[i] /= value;
    }
    return *this;
}

} // namespace matoy::foundations
