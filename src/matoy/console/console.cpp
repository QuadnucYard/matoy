#include "console.hpp"
#include "matoy/eval/eval.hpp"
#include "matoy/eval/vm.hpp"
#include <iostream>
#include <print>
#include <string>
#include <variant>

namespace matoy::console {

void Console::start() {
    std::println("Welcome to MATOY! Feel free to enter statements and get the result!");
}

void Console::loop() {
    eval::Vm vm;

    auto str = input(false);
    while (true) {
        const auto out_ = eval::try_eval_string(str, vm);
        if (!out_) {
            str += input(true);
            continue;
        }
        const auto& out = *out_;
        if (out) {
            if (!std::holds_alternative<values::none_t>(*out)) {
                std::println("{}", *out);
            }
        } else {
            for (auto& e : out.error()) {
                std::println("\033[1;31m"
                             "error: "
                             "\033[33m"
                             "source:{}: "
                             "\033[0m"
                             "{}",
                             e.span, e.message);
                for (auto& h : e.hints) {
                    std::println("  hint: {}", h);
                }
            }
        }
        str = input(false);
    }
}

std::string Console::input(bool cont) {
    if (cont) {
        std::print("\033[36m"
                   "  > "
                   "\033[0m");
    } else {
        std::print("\033[36m"
                   ">>> "
                   "\033[0m");
    }
    std::string str;
    std::getline(std::cin, str);
    return str;
}

} // namespace matoy::console
