# The C-Minus Programming Language

## Description

C-Minus is nearly a subset of the C programming language. The compiler for it is a final project of the course Compiler Principle.

## Compilation Process

```
Lexer -> Parser -> Semantic Analyzer -> C-Minus VM Assembly -> Executed Program
```

The compiler and VM are all written in C++. In order to pratice the whole process of implementation of a compiler, this project DOES NOT use tools like `lex`, `yacc` or `LLVM`. The only requirement to build it is pure C++(>=17) and `Boost` library. 

## Usage

### Test Environment

* Ubuntu 20.04 (x86-64)
* gcc (g++) 9.3.0 (x86_64-linux-gnu)
* Boost 1.71.0
* JsonCpp 1.7.4

This project uses CMake to build. So it can be built and run in Windows platform easily as well.

### Install Boost and JsonCpp

The Boost library in this project helps C++ in the function of program option and print format, etc.

```
sudo apt install libboost-dev libboost-program-options-dev
sudo apt install libjsoncpp-dev
```

### Build

```
mkdir build
cd build
cmake ..
make
```

### Run

#### Generate Assembly Code

Assume source code filename is `test.c`.

```
./cmc -i test.c -o test.s
```

`cmc` is short for C-Minus Compiler.

#### Run the Assembly Code in C-Minus VM

```
./cm -r test.s
```

`cm` is short for C-Minus.

#### Generate Assembly Code & JSON-Serialized AST File

```
./cmc -i test.c -o test.s -v sample.json
```

The JSON-Serialized AST file is outputed as `sample.json`.

