#pragma once

#include <format>
#include <initializer_list>
#include <mdspan>
#include <ranges>
#include <vector>

namespace matoy::foundations {

class Matrix {
  public:
    using value_type = double;
    using Self = Matrix;

    Matrix(size_t rows, size_t cols, std::ranges::input_range auto&& data) : rows_{rows}, cols_{cols}, data_{data} {}

    Matrix(std::initializer_list<std::initializer_list<value_type>> l);

    static Matrix zeros(size_t rows, size_t cols, const value_type& fill_value = {});

    static Matrix identity(size_t n);

    auto size() const -> size_t {
        return rows_ * cols_;
    }

    auto shape() const -> std::pair<size_t, size_t> {
        return {rows_, cols_};
    }

    auto is_square() const -> bool {
        return rows_ == cols_;
    }

    auto rows() const -> size_t {
        return rows_;
    }

    auto cols() const -> size_t {
        return cols_;
    }

    auto data() const -> const value_type* {
        return data_.data();
    }

    auto data() -> value_type* {
        return data_.data();
    }

    auto view() const {
        return std::mdspan(data_.data(), rows_, cols_);
    }

    auto view() {
        return std::mdspan(data_.data(), rows_, cols_);
    }

    Self transposed() const;

#pragma region basic_transformation

    void swap_row(size_t r1, size_t r2);

    void multiply_row(size_t r, const value_type& x);

    void add_row_multiple(size_t r1, size_t r2, const value_type& x);

#pragma endregion basic_transformation

#pragma region operators

    const value_type& operator[](size_t i, size_t j) const {
        return data_[i * cols_ + j];
    }

    value_type& operator[](size_t i, size_t j) {
        return data_[i * cols_ + j];
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

    friend bool operator==(const Self& lhs, const Self& rhs);

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
    size_t rows_;
    size_t cols_;
    std::vector<value_type> data_;

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
        for (size_t i = 0; i < mat.rows_; i++) {
            if (i != 0) {
                std::format_to(ctx.out(), "; ");
            }
            for (size_t j = 0; j < mat.cols_; j++) {
                if (j != 0) {
                    std::format_to(ctx.out(), ", ");
                }
                std::format_to(ctx.out(), "{}", mat.view()[i, j]);
            }
        }
        return std::format_to(ctx.out(), "]");
    }
};
