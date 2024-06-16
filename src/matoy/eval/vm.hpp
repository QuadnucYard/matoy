#pragma once

#include "matoy/foundations/value.hpp"
#include "matoy/syntax/span.hpp"
#include "scope.hpp"

namespace matoy::eval {

struct FlowBreak {
    syntax::Span span;
};

struct FlowContinue {
    syntax::Span span;
};

struct FlowReturn {
    syntax::Span span;
    std::optional<Value> value;
};

using FlowEvent = std::variant<FlowBreak, FlowContinue, FlowReturn>;

class Vm {
  public:
    Scopes scopes;
    std::optional<FlowEvent> flow;

    Vm() = default;
    Vm(const Vm&) = delete;
    Vm& operator=(const Vm&) = delete;

    auto define(auto&& name, auto&& value) -> void {
        scopes.top().define(std::move(name), std::move(value));
    }
};

} // namespace matoy::eval
