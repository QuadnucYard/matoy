#pragma once

#include "matoy/diag.hpp"
#include "matoy/foundations/value.hpp"

namespace matoy::eval {

template <class T>
inline auto value_cast(Value& self) -> diag::HintedResult<T*> {
    if (auto v = std::get_if<T>(&self)) {
        return v;
    } else {
        return diag::hint_error("cast failed");
    }
}

} // namespace matoy::eval
