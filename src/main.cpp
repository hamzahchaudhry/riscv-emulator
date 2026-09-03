#include "debug.hpp"
#include <print>

int main() {
  Hart hart;
  Memory mem("tests/test.bin");

  std::println("memory initialized to: {} bytes", mem.size());
  std::println();

  for (int i = 0; i < 3; ++i)
    trace_step(hart, mem);
}
