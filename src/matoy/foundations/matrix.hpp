#pragma once

#include <cassert>
#include <format>
#include <mdspan>
#include <ranges>
#include <vector>

namespace matoy::foundations {

class Matrix {
    using value_type = double;
    using Self = Matrix;

  public:
    Matrix(size_t rows, size_t cols, std::ranges::input_range auto&& data) : rows{rows}, cols{cols}, data{data} {}

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

    Self transposed() const {
        auto res{Matrix::zeros(cols, rows)};
        for (size_t i{0}; i < rows; i++) {
            for (size_t j{0}; j < cols; j++) {
                res[j, i] = (*this)[i, j];
            }
        }
        return res;
    }

#pragma region operators

    const value_type& operator[](size_t i, size_t j) const {
        return data[i * cols + j];
    }

    value_type& operator[](size_t i, size_t j) {
        return data[i * cols + j];
    }

    Self operator+() const {
        return *this;
    }

    Self operator-() const {
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
        auto res{Self::zeros(lhs.rows, rhs.cols)};
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

    friend struct std::formatter<matoy::foundations::Matrix>;
};

} // namespace matoy::foundations

#ifdef assert
#undef assert
#endif

template <>
struct std::formatter<matoy::foundations::Matrix> : std::formatter<char> {
    auto format(const matoy::foundations::Matrix& mat, format_context& ctx) const {
        std::format_to(ctx.out(), "[");
        for (size_t i = 0; i < mat.rows; i++) {
            if (i != 0) {
                std::format_to(ctx.out(), "; ");
            }
            for (size_t j = 0; j < mat.cols; j++) {
                if (j != 0) {
                    std::format_to(ctx.out(), ", ");
                }
                std::format_to(ctx.out(), "{}", mat.view()[i, j]);
            }
        }
        return std::format_to(ctx.out(), "]");
    }
};
