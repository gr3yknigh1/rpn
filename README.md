# Polish Notation

This project was build during Sber's School 21 bootcamp.

Team 70: 
  - [lambertm](https://github.com/manikala) (Leadej): worked on graph renderer and initial lexer version
  - [atruskat](https://github.com/eliot-geek): worked on infix to rpn conversion, and expression evaluation
  - [wittenbb](https://github.com/gr3yknigh1) (me): tooling, expression parser and string tokenizer, `str` and `darray` datastructures

## How to use

Compile and run. 

> NOTE: Has range from `0` to `4 PI` on X axis and from `-1` to `1` on Y axis.

Example input:

```shell
$ ./build/rpn
x * x
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
**..............................................................................
..*.............................................................................
...*............................................................................
................................................................................
....*...........................................................................
................................................................................
................................................................................
.....*..........................................................................
................................................................................
................................................................................
................................................................................
......*.........................................................................
```

## Supported operators:

  * `+` - plus
  * `-` - minus (binary and unary)
  * `*` - multiplication
  * `/` - division
  * `(` and `)` - left and right parens
  * `sin(...)` - sin
  * `cos(...)` - cos
  * `tan(...)` - tan
  * `ctg(...)` - ctg
  * `sqrt(...)` - sqrt
  * `ln(...)` - ln
  * `x` - variable

List of tokens avaiable in [token.h](./src/token.h).

## Build

Release build:

```shell
make release
```

Development build:

```shell
make all
```

## Tooling

Tests:

```shell
make test
```

Lint or format:

```shell
make lint
make fmt
```

Install git-hooks:

```shell
make hooks
```