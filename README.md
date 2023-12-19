# TiniESt
An embedded interpreter for a usable subset of the ECMAScript 5.1 standard, aiming to have an intuitive, modern C++ interface while using as little memory as possible

## Design Goals
- Intuitive C++17 interface making use of modern features
- Minimal memory usage- fit inside user-provided arena with minimal dynamically allocated stack space
- Maximal test coverage and use of safety features like const-correctness and references
- Support for as much of the ECMAScript 5 standard as possible

## Contributing
This project uses Gitflow and the [Canonical Project Structure](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html) proposed by Boris Kolpackov. This means that _all source files_ (except for tests and external libraries) lie in the `tiniest` directory. Additionally, tests use the [Catch2](https://github.com/catchorg/Catch2) unit testing framework. All new features should be covered by tests and passing before the feature branch can be merged back into main. All source files are formatted using the LLVM code style, which is automated using `clang-format`.