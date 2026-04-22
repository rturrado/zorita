# Zorita

A RISC machine implementation. Zorita means Little (Reduced) in Asturian dialect.

## Requirements

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

```
./build/Debug/src/zorita
```

## Test

```
./build/Debug/test/zorita-test
```

## Docker

Alternatively, if you have `docker` installed, you can build the application directly
(without installing any required dependency):

```
rm -fr build
docker build -t zorita .
docker run -it zorita
```

And then execute it, together with the tests:

```
$ ./build/Debug/src/zorita
$ ./build/Debug/test/zorita-test
```
