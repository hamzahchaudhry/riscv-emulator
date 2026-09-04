#include "debug.hpp"

#include <cstdlib>
#include <print>
#include <string_view>

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3) {
    std::println(stderr, "usage: riscv <test.bin> [--trace]");
    return EXIT_FAILURE;
  }

  const bool trace = argc == 3 && std::string_view{argv[2]} == "--trace";

  if (argc == 3 && !trace) {
    std::println(stderr, "unknown option: {}", argv[2]);
    return EXIT_FAILURE;
  }

  constexpr u32 memory_base = 0x80000000;

  Hart hart(memory_base);
  Memory memory(argv[1], memory_base);

  for (int i = 0; i < 100'000; ++i) {
    const auto result = trace ? trace_step(hart, memory) : hart.step(memory);

    if (result)
      continue;

    if (result.error() == Hart::Trap::EnvironmentCall) {
      // riscv-tests
      const u32 status = hart.registers().read(3);

      if (status == 1) {
        std::println("PASS");
        return EXIT_SUCCESS;
      }

      std::println("FAILED TEST: {}", status >> 1);
      return EXIT_FAILURE;
    }

    std::println("unexpected trap");
    return EXIT_FAILURE;
  }

  std::println("TIMEOUT");
  return EXIT_FAILURE;
}
