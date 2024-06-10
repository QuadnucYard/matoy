# MATOY

Naming: **MATrix TOY** or **MAth TOY**

A math interpreter toy featuring matrices written in modern CPP.

The parser implementation takes inspiration from the GREAT [Typst](https://github.com/typst/typst).

## Syntax ans Usage

```c
>>> // line comment
>>> /* block comment */
>>> 1 // int literal
1
>>> 2.3 // float literal
2.3
>>> true // boolean literal
true
>>> none // none literal
>>> a := 3 // variable declaration and definition
3
>>> a = 4 // assignment
4
>>> a += 1 // compound assignment
5
>>> a + (3 - 6) * -a // arithmetic operation
20
>>> true and false or not true // logical operation
false
>>> a > 4 and a <= 5 // compare
false
>>> a != none // compare with none
true
>>> A := [1, 2; 3, 4] // matrix
[1, 2; 3, 4]
>>> A.T // transposed
[1, 3; 2, 4]
>>> A.a // field access
error: source:0:3: type matrix does not contain field "a"
>>> B := [1
  > ,
  > (3 +
  > 4)
  > ] // multi-line input
[1, 7]
```

If the input with error only has expectation error at the end, you can continue to input the next line.

## Development

Requires: XMake, clang 19.0 with latest C++ features

Recommends: clang-format 19.0
