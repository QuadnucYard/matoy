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

    while (true) {
        const auto str = input();
        const auto out = eval::eval_string(str, vm);
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
    }
}

std::string Console::input() {
    std::print("\033[36m"
               ">>> "
               "\033[0m");
    std::string str;
    std::getline(std::cin, str);
    // TODO judge whether the input is ended
    return str;
}

} // namespace matoy::console
