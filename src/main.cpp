#include "debug.hpp"
#include <cstdlib>
#include <print>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::println(stderr, "usage: riscv <test.bin>");
    exit(EXIT_FAILURE);
  }

  constexpr u32 memory_base = 0x80000000;
  Hart hart(memory_base);
  Memory mem(argv[1], memory_base);

  for (int i = 0; i < 100'000; ++i) {
    const auto result = trace_step(hart, mem);
    if (!result) {
      if (result.error() == Hart::Trap::EnvironmentCall) {
        const u32 status = hart.registers().read(3);

        if (status == 1) {
          std::println("PASS");
          exit(EXIT_SUCCESS);
        }

        std::println("FAILED TEST: {}", status >> 1);
        exit(EXIT_FAILURE);
      }
      std::println("unexpected trap");
    }
  }
  std::println("TIMEOUT");
  exit(EXIT_FAILURE);
}
