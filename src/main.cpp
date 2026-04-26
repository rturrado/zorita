#include "zorita/Machine.hpp"
#include "zorita/Version.hpp"

#include <fmt/format.h>

#include <cstdlib> // exit
#include <filesystem>
#include <string_view>

void print_help() {
  fmt::println("Usage:                                \n"
               "  zorita -h            Show help      \n"
               "  zorita -v            Show version   \n"
               "  zorita PROGRAM_PATH  Execute program");
}

void print_version() {
  fmt::print("{} <<>> {}\n", version::get_version(),
             version::get_release_year());
}

int main(int argc, char *argv[]) {
  // Show help
  if (argc < 2 || argc > 2 ||
      std::string_view{argv[1]} ==
          "-h") { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    print_help();
    exit(0);
  }

  // Show version
  if (std::string_view{argv[1]} ==
      "-v") { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    print_version();
    exit(0);
  }

  // Execute program
  std::filesystem::path program_path{
      argv[1]}; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  try {
    zorita::Machine machine{};
    machine.load(program_path);
    machine.run();
  } catch (const zorita::MachineError &err) {
    fmt::print("Error: {}\n", err.what());
    exit(1);
  }
}
