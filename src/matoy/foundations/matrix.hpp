#pragma once

#include <cassert>
#include <mdspan>
#include <vector>

namespace matoy::foundations {

class Matrix {
    using value_type = double;
    using Self = Matrix;

  public:
    static Matrix zeros(size_t rows, size_t cols) {
        Matrix res;
        res.rows = rows;
        res.cols = cols;
        res.data.resize(rows * cols, 0.0);
        return res;
    }

    auto view() const {
        return std::mdspan(data.data(), rows, cols);
    }

    auto view() {
        return std::mdspan(data.data(), rows, cols);
    }

#pragma region operators

    Self operator+() {
        return *this;
    }

    Self operator-() {
        auto res{*this};
        for (size_t i{0}; i < data.size(); i++) {
            res.data[i] -= res.data[i];
        }
        return res;
    }

    friend Self operator+(const Self& lhs, const Self& rhs) {
        auto res{lhs};
        res += rhs;
        return res;
    }

    friend Self operator+(const Self& lhs, value_type rhs) {
        auto res{lhs};
        res += rhs;
        return res;
    }

    friend Self operator+(value_type lhs, const Self& rhs) {
        auto res{rhs};
        res += lhs;
        return res;
    }

    friend Self operator-(const Self& lhs, const Self& rhs) {
        auto res{lhs};
        res -= rhs;
        return res;
    }

    friend Self operator-(const Self& lhs, value_type rhs) {
        auto res{lhs};
        res -= rhs;
        return res;
    }

    friend Self operator-(value_type lhs, const Self& rhs) {
        auto res{rhs};
        res -= lhs;
        return res;
    }

    friend Self operator*(const Self& lhs, const Self& rhs) {
        // TODO assert
        auto res{Self::zeros(lhs.rows, lhs.cols)};
        auto v{res.view()};
        auto v1{lhs.view()};
        auto v2{rhs.view()};
        for (size_t i{0}; i < lhs.rows; i++) {
            for (size_t k{0}; i < lhs.cols; i++) {
                for (size_t j{0}; i < rhs.cols; i++) {
                    v[i, j] += v1[i, k] * v2[k, j];
                }
            }
        }
        return res;
    }

    friend Self operator*(const Self& lhs, value_type rhs) {
        auto res{lhs};
        res *= rhs;
        return res;
    }

    friend Self operator*(value_type lhs, const Self& rhs) {
        auto res{rhs};
        res *= lhs;
        return res;
    }

    friend Self operator/(const Self& lhs, value_type rhs) {
        auto res{lhs};
        res /= rhs;
        return res;
    }

    Self& operator+=(const Self& other) {
        assert_dimension(other);
        for (size_t i{0}; i < data.size(); i++) {
            data[i] += other.data[i];
        }
        return *this;
    }

    Self& operator+=(value_type value) {
        for (size_t i{0}; i < data.size(); i++) {
            data[i] += value;
        }
        return *this;
    }

    Self& operator-=(const Self& other) {
        assert_dimension(other);
        for (size_t i{0}; i < data.size(); i++) {
            data[i] -= other.data[i];
        }
        return *this;
    }

    Self& operator-=(value_type value) {
        for (size_t i{0}; i < data.size(); i++) {
            data[i] -= value;
        }
        return *this;
    }

    Self& operator*=(const Self& other) {
        return *this = *this * other;
    }

    Self& operator*=(value_type value) {
        for (size_t i{0}; i < data.size(); i++) {
            data[i] *= value;
        }
        return *this;
    }

    Self& operator/=(value_type value) {
        for (size_t i{0}; i < data.size(); i++) {
            data[i] /= value;
        }
        return *this;
    }

#pragma endregion operators

  private:
    Matrix() = default;

    void assert_dimension(const Self&) const {}

  private:
    size_t rows;
    size_t cols;
    std::vector<value_type> data;
};

} // namespace matoy::foundations

#ifdef assert
#undef assert
#endif
