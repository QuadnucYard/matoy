#pragma once

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

    auto shape() const -> std::pair<size_t, size_t> {
        return {rows, cols};
    }

    auto view() const {
        return std::mdspan(data.data(), rows, cols);
    }

    auto view() {
        return std::mdspan(data.data(), rows, cols);
    }

    Self transposed() const;

#pragma region operators

    const value_type& operator[](size_t i, size_t j) const {
        return data[i * cols + j];
    }

    value_type& operator[](size_t i, size_t j) {
        return data[i * cols + j];
    }

    Self operator+() const;

    Self operator-() const;

    friend Self operator+(const Self& lhs, const Self& rhs);

    friend Self operator+(const Self& lhs, value_type rhs);

    friend Self operator+(value_type lhs, const Self& rhs);

    friend Self operator-(const Self& lhs, const Self& rhs);

    friend Self operator-(const Self& lhs, value_type rhs);

    friend Self operator-(value_type lhs, const Self& rhs);

    friend Self operator*(const Self& lhs, const Self& rhs);

    friend Self operator*(const Self& lhs, value_type rhs);

    friend Self operator*(value_type lhs, const Self& rhs);

    friend Self operator/(const Self& lhs, value_type rhs);

    Self& operator+=(const Self& other);

    Self& operator+=(value_type value);

    Self& operator-=(const Self& other);

    Self& operator-=(value_type value);

    Self& operator*=(const Self& other);

    Self& operator*=(value_type value);

    Self& operator/=(value_type value);

#pragma endregion operators

  private:
    Matrix() = default;

  private:
    size_t rows;
    size_t cols;
    std::vector<value_type> data;

    friend struct std::formatter<matoy::foundations::Matrix>;
};

} // namespace matoy::foundations

namespace matoy {
using foundations::Matrix;
}

template <>
struct std::formatter<matoy::Matrix> : std::formatter<char> {
    auto format(const matoy::Matrix& mat, format_context& ctx) const {
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
