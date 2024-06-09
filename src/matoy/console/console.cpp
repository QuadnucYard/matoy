#include "console.hpp"
#include "matoy/eval/eval.hpp"
#include "matoy/eval/vm.hpp"
#include <iostream>
#include <print>
#include <string>

namespace matoy::console {

void Console::start() {
    std::println("Welcome to MATOY! Feel free to enter statements and get the result!");
}

void Console::loop() {
    eval::Vm vm;

    while (true) {
        auto str = input();
        auto out = eval::eval_string(str, vm);
        if (out) {
            std::println("{}", *out);
        } else {
            std::println("evaluation error!");
        }
    }
}

std::string Console::input() {
    std::print(">>> ");
    std::string str;
    std::getline(std::cin, str);
    // TODO judge whether the input is ended
    return str;
}

} // namespace matoy::console
