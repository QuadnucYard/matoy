#pragma once

#include <variant>

namespace matoy::syntax::ast {

struct Ident;
struct None;
struct Int;
struct Float;
struct Bool;

struct Code;
struct CodeBlock;
struct Parenthesized;
struct MatrixRow;
struct Matrix;
struct Unary;
struct Binary;
struct FieldAccess;
struct FuncCall;

struct Conditional;
struct WhileLoop;
struct ForLoop;
struct LoopBreak;
struct LoopContinue;
struct FuncReturn;

using Expr = std::variant<CodeBlock, Ident, None, Int, Float, Bool, Parenthesized, Matrix, Unary, Binary, FieldAccess,
                          FuncCall, Conditional, WhileLoop, ForLoop, LoopBreak, LoopContinue, FuncReturn>;

} // namespace matoy::syntax::ast
