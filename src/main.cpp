#include "debug.hpp"
#include <print>

int main() {
  Hart hart;
  Memory mem("tests/test.bin");

  std::println("memory initialized to: {} bytes", mem.size());
  std::println();

  for (int i = 0; i < 100'000; ++i) {
    const auto result = trace_step(hart, mem);
    if (!result) {
      if (result.error() == Hart::Trap::EnvironmentCall)
        break;
      std::println("unexpected trap");
    }
  }
}
