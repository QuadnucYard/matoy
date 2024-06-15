#include "matoy/foundations/matrix.hpp"
#include "matoy/foundations/matrix_op.hpp"
#include <print>

using namespace matoy;
using namespace matoy::foundations;

int main() {
    Matrix mat{{1, 2}, {3, 4}};
    std::println("{}", mat);
    std::println("{}", *inverse(mat));
    std::println("{}", mat * *inverse(mat));
    std::println("{}", *inverse(mat) * mat);
}
