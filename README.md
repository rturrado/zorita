# Zorita

A RISC machine implementation.
Zorita means Little (Reduced) in the Asturian dialect.

## Documentation
For the machine model and instruction set, see the
[Machine Model and Instruction Set](docs/pages/machine_model_and_instruction_set.md) document.

## Requirements

- A C++20-capable compiler:
  - GCC >= 10
  - Clang >= 13
  - Apple Clang >= 14
  - MSVC >= 19.2
- `CMake` >= 3.22
- `git`
- `Python` 3.x plus `pip`, with the following package:
  - `conan` >= 2.0

## Build

This version of Zorita can only be compiled via the Conan package manager.
You will need to create a default profile before using it for the first time.
The installation of dependencies, as well as the compilation, can be done in one go.

```
git clone https://github.com/rturrado/zorita.git
cd zorita
conan profile detect
conan build . -pr:a=conan/profiles/tests-debug-gcc-linux-x64 -b missing
```

## Run

Zorita doesn't build as an executable.
It just builds as a library.
And the library can only be exercised via the tests.

## Test

```
./build/Debug/test/unit/zorita-unit-test
```

## Docker

Alternatively, if you have `docker` installed, you can build directly
(without installing any required dependency):

```
rm -fr build
docker build -t zorita .
docker run -it zorita
```

And then execute the tests:

```
$ ./build/Debug/test/unit/zorita-unit-test
```
