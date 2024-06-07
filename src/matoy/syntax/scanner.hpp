#pragma once

#include <concepts>
#include <optional>
#include <string_view>

namespace matoy::syntax {

class Scanner {

  public:
    Scanner(std::string_view s) : str_{s} {}

    size_t cursor() const {
        return cursor_;
    }

    std::string_view get(size_t start, size_t end) const {
        return str_.substr(start, end - start);
    }

    std::optional<char> eat() {
        if (cursor_ == str_.length()) {
            return std::nullopt;
        } else {
            return str_[cursor_++];
        }
    }

    bool eat_if(char c) {
        if (str_[cursor_] == c) {
            cursor_++;
            return true;
        }
        return false;
    }

    bool eat_if(std::same_as<char> auto... args) {
        return (this->eat_if(args) || ...);
    }

    void eat_while(std::predicate<char> auto pred) {
        while (pred(str_[cursor_])) {
            cursor_++;
        }
    }

    void eat_until(std::predicate<char> auto pred) {
        do {
            cursor_++;
        } while (!pred(str_[cursor_]));
    }

  private:
    std::string_view str_;
    size_t cursor_{0};
};

} // namespace matoy::syntax
