#include <fmt/format.h>
#include <iostream>

#include "zorita/version.hpp"

void print_version() {
    fmt::print("{} <<>> {}\n", version::get_version(), version::get_release_year());
}

int main() {
    try {
        print_version();
    } catch (const std::runtime_error& error) {
        fmt::print("Error: {}", error.what());
    }
}
