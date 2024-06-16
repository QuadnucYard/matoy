#include "matoy/eval/cast.hpp"
#include "matoy/eval/fwd.hpp"
#include "matoy/eval/vm.hpp"
#include "matoy/syntax/ast.hpp"

namespace matoy::eval {

template <>
auto eval(const ast::Conditional& self, Vm& vm) -> diag::SourceResult<Value> {
    const auto condition = self.condition();
    const auto cond = eval(condition, vm);
    if (!cond)
        return cond;

    if (auto b = std::get_if<bool>(&*cond)) {
        if (*b) {
            return eval(self.if_body(), vm);
        } else if (auto else_body = self.else_body()) {
            return eval(*else_body, vm);
        } else {
            return values::None{};
        }
    } else {
        return diag::source_error(syntax::get_span(condition), "casting to boolean is not supported yet");
    }
}

template <>
auto eval(const ast::WhileLoop& self, Vm& vm) -> diag::SourceResult<Value> {
    const auto flow = std::move(vm.flow);
    const auto condition = self.condition();
    const auto body = self.body();

    while (true) {
        auto cond = eval(condition, vm);
        if (!cond)
            return cond;
        auto cond_b = diag::to_source_error(value_cast<bool>(*cond), syntax::get_span(condition));
        if (!cond_b)
            return std::move(cond_b).transform([](auto&& x) { return *x; });
        if (!**cond_b)
            break;

        const auto value = eval(body, vm);

        if (!flow) {
            // do nothing
        } else if (std::holds_alternative<FlowBreak>(*flow)) {
            vm.flow = std::nullopt;
            break;
        } else if (std::holds_alternative<FlowContinue>(*flow)) {
            vm.flow = std::nullopt;
        } else if (std::holds_alternative<FlowReturn>(*flow)) {
            break;
        }
    }

    if (flow) {
        vm.flow = flow;
    }
    return values::None{};
}

template <>
auto eval(const ast::ForLoop&, Vm&) -> diag::SourceResult<Value> {
    return {};
}

template <>
auto eval(const ast::LoopBreak& self, Vm& vm) -> diag::SourceResult<Value> {
    if (!vm.flow) {
        vm.flow = FlowBreak{self.span()};
    }
    return values::None{};
}

template <>
auto eval(const ast::LoopContinue& self, Vm& vm) -> diag::SourceResult<Value> {
    if (!vm.flow) {
        vm.flow = FlowContinue{self.span()};
    }
    return values::None{};
}

template <>
auto eval(const ast::FuncReturn& self, Vm& vm) -> diag::SourceResult<Value> {
    auto value = self.body().transform([&vm](auto&& v) { return eval(v, vm); });
    if (value && !*value) {
        return std::move(*value);
    }
    if (!vm.flow) {
        vm.flow = FlowReturn{self.span(), value.transform([](auto&& x) { return std::move(*x); })};
    }
    return values::None{};
}

} // namespace matoy::eval
