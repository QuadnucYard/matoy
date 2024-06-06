#pragma once

#include <concepts>
#include <sstream>
#include <string_view>

namespace matoy::syntax {

class Scanner {

  public:
    Scanner(const std::istream& s) { this->s << s.rdbuf(); }
    Scanner(std::string_view s) { this->s << s; }

    size_t cursor() { return this->s.tellg(); }

    std::string_view get(size_t start, size_t end) const { return this->s.view().substr(start, end - start); }

    std::optional<char> eat() {
        char c = this->s.get();
        if (this->s) { // avoid reading EOF
            return c;
        } else {
            return std::nullopt;
        }
    }

    bool eat_if(char c) {
        if (this->s.peek() == c) {
            this->s.get();
            return true;
        }
        return false;
    }

    template <std::same_as<char>... Args>
    bool eat_if(Args... args) {
        return (this->eat_if(args) || ...);
    }

    template <std::predicate<char> Pred>
    void eat_while(Pred pred) {
        while (pred(this->s.peek())) {
            this->s.get();
        }
    }

    template <std::predicate<char> Pred>
    void eat_until(Pred pred) {
        do {
            this->s.get();
        } while (!pred(this->s.peek()));
    }

  private:
    std::stringstream s;
};

} // namespace matoy::syntax
